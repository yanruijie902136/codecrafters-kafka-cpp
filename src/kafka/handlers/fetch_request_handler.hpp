#ifndef codecrafters_kafka_handlers_fetch_request_handler_hpp
#define codecrafters_kafka_handlers_fetch_request_handler_hpp

#include <string>

#include "kafka/handlers/request_handler.hpp"
#include "kafka/requests/fetch_request.hpp"
#include "kafka/requests/fetch_response.hpp"

namespace kafka {

class FetchRequestHandler : public RequestHandler {
private:
        std::unique_ptr<Response> build_response_body(const Request *request) override;

        FetchableTopicResponse process_fetch_topic(const FetchTopic &fetch_topic);
        PartitionData fetch_topic_partition(const std::string &topic_name, std::int32_t partition_id);
};

}

#endif
