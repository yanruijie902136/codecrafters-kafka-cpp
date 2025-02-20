#include "kafka/handlers/api_versions_request_handler.hpp"
#include "kafka/protocol/api_key.hpp"
#include "kafka/protocol/error_code.hpp"
#include "kafka/requests/api_versions_response.hpp"

#include <cstdint>
#include <memory>
#include <utility>

namespace kafka {

std::unique_ptr<Response> ApiVersionsRequestHandler::build_response_body(const Request *request) {
        ApiVersionsResponse response;

        std::int16_t request_api_version = request->header().request_api_version();
        if (request_api_version == 4) {
                response.set_error_code(ErrorCode::NONE);
        } else {
                response.set_error_code(ErrorCode::UNSUPPORTED_VERSION);
        }

        response.set_api_keys({
                {ApiKey::FETCH, 16, 16},
                {ApiKey::API_VERSIONS, 4, 4},
                {ApiKey::DESCRIBE_TOPIC_PARTITIONS, 0, 0},
        });

        response.set_throttle_time_ms(0);

        return std::make_unique<ApiVersionsResponse>(std::move(response));
}

}
