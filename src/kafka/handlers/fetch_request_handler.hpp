#ifndef codecrafters_kafka_handlers_fetch_request_handler_hpp
#define codecrafters_kafka_handlers_fetch_request_handler_hpp

#include "kafka/handlers/request_handler.hpp"
#include "kafka/requests/fetch_request.hpp"
#include "kafka/requests/fetch_response.hpp"

namespace kafka {

class FetchRequestHandler : public RequestHandler {
private:
        std::unique_ptr<Response> build_response_body(const Request *request) override;

        FetchableTopicResponse process_fetch_topic(const FetchTopic &fetch_topic);
};

}

#endif
