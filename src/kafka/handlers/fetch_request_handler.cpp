#include "kafka/handlers/fetch_request_handler.hpp"
#include "kafka/protocol/error_code.hpp"
#include "kafka/requests/fetch_response.hpp"

#include <memory>
#include <utility>

namespace kafka {

std::unique_ptr<Response> FetchRequestHandler::build_response_body(const Request *request) {
        FetchResponse response;

        response.set_throttle_time_ms(0);
        response.set_error_code(ErrorCode::NONE);
        response.set_session_id(0);

        return std::make_unique<FetchResponse>(std::move(response));
}

}
