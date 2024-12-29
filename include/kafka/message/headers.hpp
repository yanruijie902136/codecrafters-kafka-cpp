#ifndef CODECRAFTERS_KAFKA_MESSAGE_HEADERS_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_MESSAGE_HEADERS_HPP_INCLUDED

#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/iwritable.hpp"
#include "kafka/protocol/types.hpp"

namespace kafka {

class RequestHeader {
public:
    // Reads this `RequestHeader` from a byte stream.
    void read(IReadable &readable) {
        request_api_key_ = read_api_key(readable);
        request_api_version_ = read_int16(readable);
        correlation_id_ = read_int32(readable);
        client_id_ = read_nullable_string(readable);
        read_tagged_fields(readable);
    }

    const ApiKey &request_api_key() const {
        return request_api_key_;
    }

    const INT16 &request_api_version() const {
        return request_api_version_;
    }

    const INT32 &correlation_id() const {
        return correlation_id_;
    }

private:
    ApiKey request_api_key_;
    INT16 request_api_version_;
    INT32 correlation_id_;
    NULLABLE_STRING client_id_;
};

class ResponseHeader {
public:
    explicit ResponseHeader(int correlation_id) : correlation_id_(correlation_id) {}

    // Writes this `ResponseHeader` to a byte stream.
    void write(IWritable &writable, short version) const {
        write_int32(writable, correlation_id_);
        if (version != 0) {
            write_tagged_fields(writable);
        }
    }

private:
    INT32 correlation_id_;
};

}

#endif  // CODECRAFTERS_KAFKA_MESSAGE_HEADERS_HPP_INCLUDED
