#include "kafka/handlers/describe_topic_partitions_request_handler.hpp"
#include "kafka/metadata/cluster_metadata.hpp"
#include "kafka/protocol/uuid.hpp"
#include "kafka/requests/describe_topic_partitions_request.hpp"
#include "kafka/requests/describe_topic_partitions_response.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace kafka {

std::unique_ptr<Response> DescribeTopicPartitionsRequestHandler::build_response_body(const Request *arg) {
        const auto *request = dynamic_cast<const DescribeTopicPartitionsRequest *>(arg);

        DescribeTopicPartitionsResponse response;

        response.set_throttle_time_ms(0);

        std::vector<ResponseTopic> topics;
        for (const auto &topic_request : request->topics()) {
                topics.push_back(process_topic_request(topic_request));
        }
        response.set_topics(std::move(topics));

        return std::make_unique<DescribeTopicPartitionsResponse>(std::move(response));
}

ResponseTopic DescribeTopicPartitionsRequestHandler::process_topic_request(const TopicRequest &topic_request) {
        ResponseTopic response_topic;
        response_topic.set_name(topic_request.name());

        try {
                auto &cluster_metadata = ClusterMetadata::instance();

                Uuid topic_id = cluster_metadata.lookup_topic_id(topic_request.name());
                response_topic.set_topic_id(topic_id);

                std::vector<ResponsePartition> partitions;
                for (std::int32_t partition_id : cluster_metadata.lookup_partitions(topic_id)) {
                        partitions.emplace_back(ErrorCode::NONE, partition_id);
                }
                response_topic.set_partitions(std::move(partitions));
        } catch (...) {
                response_topic.set_error_code(ErrorCode::UNKNOWN_TOPIC_OR_PARTITION);
                return response_topic;
        }

        response_topic.set_error_code(ErrorCode::NONE);
        return response_topic;
}

}
