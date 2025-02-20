#ifndef codecrafters_kafka_requests_response_header_hpp
#define codecrafters_kafka_requests_response_header_hpp

#include <cstdint>

#include "kafka/protocol/writable.hpp"

namespace kafka {

// Response header version 0~1.
class ResponseHeader {
public:
        // Writes this response header to a byte stream.
        void write(Writable &writable, short version) const {
                write_int32(writable, correlation_id_);
                if (version == 1) {
                        write_tagged_fields(writable);
                }
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
