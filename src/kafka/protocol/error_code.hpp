#ifndef codecrafters_kafka_protocol_error_code_hpp
#define codecrafters_kafka_protocol_error_code_hpp

#include <cstdint>

#include "kafka/protocol/writable.hpp"

namespace kafka {

// Numeric codes that indicate what problem occurred on the server.
enum class ErrorCode : std::int16_t {
        NONE = 0,
        // This server does not host this topic-partition.
        UNKNOWN_TOPIC_OR_PARTITION = 3,
        // The version of API is not supported.
        UNSUPPORTED_VERSION = 35,
        // This server does not host this topic ID.
        UNKNOWN_TOPIC_ID = 100,
};

// Writes an ErrorCode to a byte stream.
void write_error_code(Writable &writable, ErrorCode error_code);

}

#endif
