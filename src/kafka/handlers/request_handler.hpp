#ifndef codecrafters_kafka_handlers_request_handler_hpp
#define codecrafters_kafka_handlers_request_handler_hpp

#include <memory>
#include <utility>

#include "kafka/requests/request.hpp"
#include "kafka/requests/request_header.hpp"
#include "kafka/requests/response.hpp"
#include "kafka/requests/response_header.hpp"

namespace kafka {

// Abstract base class of an API request handler.
class RequestHandler {
public:
        virtual ~RequestHandler() {}

        std::unique_ptr<Response> handle_request(const Request *request) {
                auto response_header = build_response_header(request->header());
                auto response = build_response_body(request);
                response->set_response_header(std::move(response_header));
                return response;
        }

private:
        ResponseHeader build_response_header(const RequestHeader &request_header) {
                ResponseHeader response_header;
                response_header.set_correlation_id(request_header.correlation_id());
                return response_header;
        }

        virtual std::unique_ptr<Response> build_response_body(const Request *request) = 0;
};

// Instantiate the corresponding request handler.
std::unique_ptr<RequestHandler> choose_request_handler(const Request *request);

}

#endif
