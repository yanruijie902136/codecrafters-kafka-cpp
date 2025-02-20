#ifndef codecrafters_kafka_requests_api_versions_response_hpp
#define codecrafters_kafka_requests_api_versions_response_hpp

#include <cstdint>
#include <utility>
#include <vector>

#include "kafka/protocol/api_key.hpp"
#include "kafka/protocol/error_code.hpp"
#include "kafka/protocol/writable.hpp"
#include "kafka/requests/response.hpp"

namespace kafka {

class ApiVersion {
public:
        ApiVersion(ApiKey api_key, std::int16_t min_version, std::int16_t max_version) : api_key_(api_key), min_version_(min_version), max_version_(max_version) {}

        void write(Writable &writable) const {
                write_api_key(writable, api_key_);
                write_int16(writable, min_version_);
                write_int16(writable, max_version_);
                write_tagged_fields(writable);
        }

private:
        ApiKey api_key_;
        std::int16_t min_version_;
        std::int16_t max_version_;
};

// ApiVersions response version 4.
class ApiVersionsResponse : public Response {
public:
        // Sets the top-level error code.
        void set_error_code(ErrorCode error_code) {
                error_code_ = error_code;
        }

        // Sets the APIs supported by the broker.
        void set_api_keys(std::vector<ApiVersion> api_keys) {
                api_keys_ = std::move(api_keys);
        }

        // Sets the duration in milliseconds for which the request was throttled due to a quota violation, or zero if the request did not violate any quota.
        void set_throttle_time_ms(std::int32_t throttle_time_ms) {
                throttle_time_ms_ = throttle_time_ms;
        }

private:
        ErrorCode error_code_;
        std::vector<ApiVersion> api_keys_;
        std::int32_t throttle_time_ms_;

        void write_response_header(Writable &writable) const override {
                // ApiVersions response uses response header version 0.
                header_.write(writable, 0);
        }

        void write_response_body(Writable &writable) const override {
                write_error_code(writable, error_code_);
                write_compact_array(writable, api_keys_);
                write_int32(writable, throttle_time_ms_);
                write_tagged_fields(writable);
        }
};

}

#endif
