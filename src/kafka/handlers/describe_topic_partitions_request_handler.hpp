#ifndef codecrafters_kafka_handlers_describe_topic_partitions_request_handler_hpp
#define codecrafters_kafka_handlers_describe_topic_partitions_request_handler_hpp

#include "kafka/handlers/request_handler.hpp"
#include "kafka/requests/describe_topic_partitions_request.hpp"
#include "kafka/requests/describe_topic_partitions_response.hpp"

namespace kafka {

class DescribeTopicPartitionsRequestHandler : public RequestHandler {
private:
        std::unique_ptr<Response> build_response_body(const Request *request) override;

        ResponseTopic process_topic_request(const TopicRequest &topic_request);
};

}

#endif
