#include "kafka/network/server.hpp"
#include "kafka/message/abstract.hpp"
#include "kafka/message/api_versions.hpp"
#include "kafka/message/describe_topic_partitions.hpp"
#include "kafka/message/headers.hpp"
#include "kafka/message/messages.hpp"
#include "kafka/network/client.hpp"
#include "kafka/protocol/constants.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>
#include <thread>

namespace kafka {

Server::Server() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        throw std::system_error(errno, std::system_category(), "socket");
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors.
    const int reuse = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        throw std::system_error(errno, std::system_category(), "setsockopt");
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(9092);
    if (bind(server_socket_, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
        throw std::system_error(errno, std::system_category(), "bind");
    }

    const int backlog = 5;
    if (listen(server_socket_, backlog) < 0) {
        throw std::system_error(errno, std::system_category(), "listen");
    }
}

static std::unique_ptr<AbstractResponse> handle_api_versions(const RequestMessage &request_message) {
    const ApiVersionsRequest *request = request_message.request<ApiVersionsRequest>();

    ApiVersionsResponse response;
    if (request_message.header().request_api_version() != 4) {
        response.error_code() = ErrorCode::UNSUPPORTED_VERSION;
    } else {
        response.error_code() = ErrorCode::NONE;
        response.api_keys().emplace_back(ApiKey::API_VERSIONS, 0, 4);
        response.api_keys().emplace_back(ApiKey::DESCRIBE_TOPIC_PARTITIONS, 0, 0);
    }
    response.throttle_time_ms() = 0;

    return std::make_unique<ApiVersionsResponse>(std::move(response));
}

static std::unique_ptr<AbstractResponse> handle_describe_topic_partitions(const RequestMessage &request_message) {
    const DescribeTopicPartitionsRequest *request = request_message.request<DescribeTopicPartitionsRequest>();

    DescribeTopicPartitionsResponse response;
    response.throttle_time_ms() = 0;
    for (const auto &topic_request : request->topics()) {
        using ResponseTopic = DescribeTopicPartitionsResponse::ResponseTopic;

        ResponseTopic response_topic;
        response_topic.error_code() = ErrorCode::UNKNOWN_TOPIC_OR_PARTITION;
        response_topic.name() = topic_request.name();

        response.topics().push_back(std::move(response_topic));
    }

    return std::make_unique<DescribeTopicPartitionsResponse>(std::move(response));
}

static void serve_client(Client client) {
    for ( ; ; ) {
        auto request_message = client.read_request();

        ResponseHeader response_header(request_message.header().correlation_id());
        std::unique_ptr<AbstractResponse> response;
        switch (request_message.header().request_api_key()) {
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
            throw std::system_error(errno, std::system_category(), "accept");
        }

        Client client(client_socket);
        std::thread t(serve_client, std::move(client));
        t.detach();
    }
}

}
