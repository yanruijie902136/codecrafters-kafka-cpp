#include "kafka/handlers/fetch_request_handler.hpp"
#include "kafka/protocol/error_code.hpp"
#include "kafka/requests/fetch_request.hpp"
#include "kafka/requests/fetch_response.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace kafka {

std::unique_ptr<Response> FetchRequestHandler::build_response_body(const Request *arg) {
        const auto *request = dynamic_cast<const FetchRequest *>(arg);

        FetchResponse response;

        response.set_throttle_time_ms(0);
        response.set_error_code(ErrorCode::NONE);
        response.set_session_id(0);

        std::vector<FetchableTopicResponse> responses;
        for (const auto &fetch_topic : request->topics()) {
                responses.push_back(process_fetch_topic(fetch_topic));
        }
        response.set_responses(std::move(responses));

        return std::make_unique<FetchResponse>(std::move(response));
}

FetchableTopicResponse FetchRequestHandler::process_fetch_topic(const FetchTopic &fetch_topic) {
        FetchableTopicResponse topic_response;

        topic_response.set_topic_id(fetch_topic.topic_id());
        topic_response.set_partitions({{0, ErrorCode::UNKNOWN_TOPIC_ID}});

        return topic_response;
}

}
