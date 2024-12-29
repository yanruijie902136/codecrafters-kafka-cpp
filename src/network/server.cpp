#include "kafka/network/server.hpp"
#include "kafka/message/abstract.hpp"
#include "kafka/message/api_versions.hpp"
#include "kafka/message/describe_topic_partitions.hpp"
#include "kafka/message/fetch.hpp"
#include "kafka/message/headers.hpp"
#include "kafka/message/messages.hpp"
#include "kafka/metadata/cluster_metadata.hpp"
#include "kafka/network/client.hpp"
#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/utils.hpp"

#include <cerrno>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>

namespace kafka {

Server::Server() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        throw_system_error("socket");
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors.
    const int reuse = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        throw_system_error("setsockopt");
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = to_network_byte_order(INADDR_ANY);
    server_addr.sin_port = to_network_byte_order((unsigned short)9092);
    if (bind(server_socket_, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
        throw_system_error("bind");
    }

    const int backlog = 5;
    if (listen(server_socket_, backlog) < 0) {
        throw_system_error("listen");
    }
}

using FetchTopic = FetchRequest::FetchTopic;
using FetchableTopicResponse = FetchResponse::FetchableTopicResponse;
using PartitionData = FetchResponse::PartitionData;

static PartitionData make_partition_data(const std::string &topic_name, INT32 partition_index) {
    PartitionData partition_data;
    partition_data.partition_index() = partition_index;
    partition_data.error_code() = ErrorCode::NONE;
    partition_data.records() = read_record_batches(topic_name, partition_index);
    return partition_data;
}

static FetchableTopicResponse make_fetchable_topic_response(const FetchTopic &fetch_topic) {
    FetchableTopicResponse res;
    UUID topic_id = fetch_topic.topic_id();
    res.topic_id() = topic_id;

    const auto &cluster_metadata = ClusterMetadata::get_instance();
    try {
        auto topic_name = cluster_metadata.get_topic_name(topic_id);
        for (const auto &fetch_partition : fetch_topic.partitions()) {
            INT32 partition_index = fetch_partition.partition();
            res.partitions().push_back(make_partition_data(topic_name, partition_index));
        }
    } catch (...) {
        PartitionData partition_data;
        partition_data.partition_index() = 0;
        partition_data.error_code() = ErrorCode::UNKNOWN_TOPIC_ID;
        res.partitions().push_back(std::move(partition_data));
    }

    return res;
}

static std::unique_ptr<FetchResponse> handle_fetch(const RequestMessage &request_message) {
    const FetchRequest *request = request_message.request<FetchRequest>();

    FetchResponse response;
    response.error_code() = ErrorCode::NONE;
    response.throttle_time_ms() = 0;
    response.session_id() = 0;
    for (const auto &fetch_topic : request->topics()) {
        response.responses().push_back(make_fetchable_topic_response(fetch_topic));
    }

    return std::make_unique<FetchResponse>(std::move(response));
}

static std::unique_ptr<ApiVersionsResponse> handle_api_versions(const RequestMessage &request_message) {
    const ApiVersionsRequest *request = request_message.request<ApiVersionsRequest>();

    ApiVersionsResponse response;
    if (request_message.header().request_api_version() != 4) {
        response.error_code() = ErrorCode::UNSUPPORTED_VERSION;
    } else {
        response.error_code() = ErrorCode::NONE;
        response.api_keys().emplace_back(ApiKey::FETCH, 0, 16);
        response.api_keys().emplace_back(ApiKey::API_VERSIONS, 0, 4);
        response.api_keys().emplace_back(ApiKey::DESCRIBE_TOPIC_PARTITIONS, 0, 0);
    }
    response.throttle_time_ms() = 0;

    return std::make_unique<ApiVersionsResponse>(std::move(response));
}

using TopicRequest = DescribeTopicPartitionsRequest::TopicRequest;
using ResponseTopic = DescribeTopicPartitionsResponse::ResponseTopic;

static ResponseTopic make_response_topic(const TopicRequest &topic_request) {
    ResponseTopic response_topic;
    response_topic.name() = topic_request.name();

    const ClusterMetadata &cluster_metadata = ClusterMetadata::get_instance();
    UUID topic_id;
    try {
        topic_id = cluster_metadata.get_topic_id(topic_request.name());
    } catch (...) {
        response_topic.error_code() = ErrorCode::UNKNOWN_TOPIC_OR_PARTITION;
        return response_topic;
    }
    response_topic.error_code() = ErrorCode::NONE;
    response_topic.topic_id() = topic_id;
    for (INT32 partition_id : cluster_metadata.get_partition_ids(topic_id)) {
        response_topic.partitions().emplace_back(ErrorCode::NONE, partition_id);
    }

    return response_topic;
}

static std::unique_ptr<DescribeTopicPartitionsResponse> handle_describe_topic_partitions(const RequestMessage &request_message) {
    const DescribeTopicPartitionsRequest *request = request_message.request<DescribeTopicPartitionsRequest>();

    DescribeTopicPartitionsResponse response;
    response.throttle_time_ms() = 0;
    for (const auto &topic_request : request->topics()) {
        response.topics().push_back(make_response_topic(topic_request));
    }

    return std::make_unique<DescribeTopicPartitionsResponse>(std::move(response));
}

static void serve_client(Client client) {
    for ( ; ; ) {
        auto request_message = client.read_request();

        ResponseHeader response_header(request_message.header().correlation_id());
        std::unique_ptr<AbstractResponse> response;
        switch (request_message.header().request_api_key()) {
            case ApiKey::FETCH:
                response = handle_fetch(request_message);
                break;
            case ApiKey::API_VERSIONS:
                response = handle_api_versions(request_message);
                break;
            case ApiKey::DESCRIBE_TOPIC_PARTITIONS:
                response = handle_describe_topic_partitions(request_message);
                break;
        }

        client.write_response(ResponseMessage(std::move(response_header), std::move(response)));
    }
}

void Server::start() {
    for ( ; ; ) {
        int client_socket = accept(server_socket_, nullptr, nullptr);
        if (client_socket < 0) {
            if (errno == ECONNABORTED || errno == EINTR) {
                continue;
            }
            throw_system_error("accept");
        }

        Client client(client_socket);
        std::thread t(serve_client, std::move(client));
        t.detach();
    }
}

}
