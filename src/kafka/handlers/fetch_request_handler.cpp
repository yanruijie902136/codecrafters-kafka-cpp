#include "kafka/handlers/fetch_request_handler.hpp"
#include "kafka/metadata/cluster_metadata.hpp"
#include "kafka/metadata/record_batch.hpp"
#include "kafka/protocol/error_code.hpp"
#include "kafka/requests/fetch_request.hpp"
#include "kafka/requests/fetch_response.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace kafka {

std::unique_ptr<Response> FetchRequestHandler::build_response_body(const Request *arg) {
        const auto *request = dynamic_cast<const FetchRequest *>(arg);

        FetchResponse response;

        response.set_throttle_time_ms(0);
        response.set_error_code(ErrorCode::NONE);
        response.set_session_id(0);

        std::vector<FetchableTopicResponse> responses;
        for (const auto &fetch_topic : request->topics()) {
                responses.push_back(process_fetch_topic(fetch_topic));
        }
        response.set_responses(std::move(responses));

        return std::make_unique<FetchResponse>(std::move(response));
}

FetchableTopicResponse FetchRequestHandler::process_fetch_topic(const FetchTopic &fetch_topic) {
        FetchableTopicResponse topic_response;

        const Uuid &topic_id = fetch_topic.topic_id();
        topic_response.set_topic_id(topic_id);

        // Check the topic's existence.
        auto &cluster_metadata = ClusterMetadata::instance();
        std::vector<std::int32_t> partition_ids;
        try {
                partition_ids = cluster_metadata.lookup_partitions(topic_id);
        } catch (...) {
                std::vector<PartitionData> partitions(1);
                partitions[0].set_partition_index(0);
                partitions[0].set_error_code(ErrorCode::UNKNOWN_TOPIC_ID);
                topic_response.set_partitions(std::move(partitions));
                return topic_response;
        }

        // Read the topic's partitions.
        std::vector<PartitionData> partitions;
        for (std::int32_t partition_id : partition_ids) {
                partitions.push_back(read_topic_partition(topic_id, partition_id));
        }
        topic_response.set_partitions(std::move(partitions));

        return topic_response;
}

PartitionData FetchRequestHandler::read_topic_partition(const Uuid &topic_id, std::int32_t partition_id) {
        PartitionData partition_data;

        partition_data.set_partition_index(partition_id);
        partition_data.set_error_code(ErrorCode::NONE);

        auto &cluster_metadata = ClusterMetadata::instance();
        std::string topic_name = cluster_metadata.lookup_topic_name(topic_id);

        auto record_batches = read_record_batches(topic_name, partition_id);
        partition_data.set_records(std::move(record_batches));

        return partition_data;
}

}
