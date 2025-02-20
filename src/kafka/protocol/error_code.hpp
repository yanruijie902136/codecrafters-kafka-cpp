#ifndef codecrafters_kafka_protocol_error_code_hpp
#define codecrafters_kafka_protocol_error_code_hpp

#include <cstdint>

#include "kafka/protocol/writable.hpp"

namespace kafka {

// Numeric codes that indicate what problem occurred on the server.
enum class ErrorCode : std::int16_t {
        NONE = 0,
        // The version of API is not supported.
        UNSUPPORTED_VERSION = 35,
};

// Writes an ErrorCode to a byte stream.
void write_error_code(Writable &writable, ErrorCode error_code);

}

#endif
