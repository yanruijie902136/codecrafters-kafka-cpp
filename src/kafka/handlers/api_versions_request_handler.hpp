#ifndef codecrafters_kafka_handlers_api_versions_request_handler_hpp
#define codecrafters_kafka_handlers_api_versions_request_handler_hpp

#include "kafka/handlers/request_handler.hpp"

namespace kafka {

class ApiVersionsRequestHandler : public RequestHandler {
private:
        std::unique_ptr<Response> build_response_body(const Request *request) override;
};

}

#endif
