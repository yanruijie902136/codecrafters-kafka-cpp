#include "kafka/handlers/request_handler.hpp"
#include "kafka/handlers/api_versions_request_handler.hpp"

#include <memory>

namespace kafka {

std::unique_ptr<RequestHandler> choose_request_handler(const Request *request) {
        switch (request->header().request_api_key()) {
        case ApiKey::API_VERSIONS:
                return std::make_unique<ApiVersionsRequestHandler>();
        }
}

}
