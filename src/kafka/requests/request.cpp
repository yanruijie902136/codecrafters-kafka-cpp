#include "kafka/requests/request.hpp"
#include "kafka/requests/api_versions_request.hpp"
#include "kafka/requests/describe_topic_partitions_request.hpp"
#include "kafka/requests/request_header.hpp"

#include <memory>
#include <utility>

namespace kafka {

std::unique_ptr<Request> Request::read(Readable &readable) {
        RequestHeader header;
        header.read(readable);

        std::unique_ptr<Request> request;
        switch (header.request_api_key()) {
        case ApiKey::API_VERSIONS:
                request = std::make_unique<ApiVersionsRequest>();
                break;
        case ApiKey::DESCRIBE_TOPIC_PARTITIONS:
                request = std::make_unique<DescribeTopicPartitionsRequest>();
                break;
        }
        request->read_request_body(readable);

        request->header_ = std::move(header);

        return request;
}

}
