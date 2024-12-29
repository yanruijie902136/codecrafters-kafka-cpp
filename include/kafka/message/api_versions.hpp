#ifndef CODECRAFTERS_KAFKA_MESSAGE_API_VERSIONS_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_MESSAGE_API_VERSIONS_HPP_INCLUDED

#include "kafka/message/abstract.hpp"
#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/iwritable.hpp"
#include "kafka/protocol/types.hpp"

namespace kafka {

class ApiVersionsRequest : public AbstractRequest {
public:
    // Reads this `ApiVersionsRequest` from a byte stream.
    void read(IReadable &readable) override {
        client_software_name_ = read_compact_string(readable);
        client_software_version_ = read_compact_string(readable);
        read_tagged_fields(readable);
    }

private:
    COMPACT_STRING client_software_name_;
    COMPACT_STRING client_software_version_;
};

class ApiVersionsResponse : public AbstractResponse {
public:
    class ApiVersion {
    public:
        ApiVersion(ApiKey api_key, INT16 min_version, INT16 max_version)
            : api_key_(api_key), min_version_(min_version), max_version_(max_version) {}

        // Writes this `ApiVersion` to a byte stream.
        void write(IWritable &writable) const {
            write_api_key(writable, api_key_);
            write_int16(writable, min_version_);
            write_int16(writable, max_version_);
            write_tagged_fields(writable);
        }

    private:
        ApiKey api_key_;
        INT16 min_version_;
        INT16 max_version_;
    };

    // The API key of this `ApiVersionsResponse`.
    constexpr ApiKey api_key() const override {
        return ApiKey::API_VERSIONS;
    }

    // Writes this `ApiVersionsResponse` to a byte stream.
    void write(IWritable &writable) const override {
        write_error_code(writable, error_code_);
        write_compact_array(writable, api_keys_);
        write_int32(writable, throttle_time_ms_);
        write_tagged_fields(writable);
    }

    ErrorCode &error_code() {
        return error_code_;
    }

    COMPACT_ARRAY<ApiVersion> &api_keys() {
        return api_keys_;
    }

    INT32 &throttle_time_ms() {
        return throttle_time_ms_;
    }

private:
    ErrorCode error_code_;
    COMPACT_ARRAY<ApiVersion> api_keys_;
    INT32 throttle_time_ms_;
};

}

#endif  // CODECRAFTERS_KAFKA_MESSAGE_API_VERSIONS_HPP_INCLUDED
