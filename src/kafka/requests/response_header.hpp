#ifndef codecrafters_kafka_requests_response_header_hpp
#define codecrafters_kafka_requests_response_header_hpp

#include <cstdint>

#include "kafka/protocol/writable.hpp"

namespace kafka {

// Response header version 0.
class ResponseHeader {
public:
        // Writes this response header to a byte stream.
        void write(Writable &writable) const {
                write_int32(writable, correlation_id_);
        }

        // Sets the correlation ID of this response.
        void set_correlation_id(std::int32_t correlation_id) {
                correlation_id_ = correlation_id;
        }

private:
        std::int32_t correlation_id_;
};

}

#endif
