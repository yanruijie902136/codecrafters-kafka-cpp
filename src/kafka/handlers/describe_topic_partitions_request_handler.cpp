#include "kafka/handlers/describe_topic_partitions_request_handler.hpp"
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
                topics.push_back(build_response_topic(topic_request));
        }
        response.set_topics(std::move(topics));

        return std::make_unique<DescribeTopicPartitionsResponse>(std::move(response));
}

ResponseTopic DescribeTopicPartitionsRequestHandler::build_response_topic(const TopicRequest &topic_request) {
        ResponseTopic response_topic;
        response_topic.set_error_code(ErrorCode::UNKNOWN_TOPIC_OR_PARTITION);
        response_topic.set_name(topic_request.name());
        return response_topic;
}

}
