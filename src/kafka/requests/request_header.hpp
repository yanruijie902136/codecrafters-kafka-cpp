#ifndef codecrafters_kafka_requests_request_header_hpp
#define codecrafters_kafka_requests_request_header_hpp

#include <cstdint>
#include <string>

#include "kafka/protocol/api_key.hpp"
#include "kafka/protocol/readable.hpp"

namespace kafka {

// Request header version 2.
class RequestHeader {
public:
        // Reads this request header from a byte stream.
        void read(Readable &readable) {
                request_api_key_ = read_api_key(readable);
                request_api_version_ = read_int16(readable);
                correlation_id_ = read_int32(readable);
                client_id_ = read_nullable_string(readable);
                read_tagged_fields(readable);
        }

        // The API key of this request.
        ApiKey request_api_key() const {
                return request_api_key_;
        }

        // The API version of this request.
        std::int16_t request_api_version() const {
                return request_api_version_;
        }

        // The correlation ID of this request.
        std::int32_t correlation_id() const {
                return correlation_id_;
        }

private:
        ApiKey request_api_key_;
        std::int16_t request_api_version_;
        std::int32_t correlation_id_;
        std::string client_id_;
};

}

#endif
