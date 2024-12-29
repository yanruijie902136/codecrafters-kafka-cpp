#ifndef CODECRAFTERS_KAFKA_PROTOCOL_CONSTANTS_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_CONSTANTS_HPP_INCLUDED

#include "kafka/protocol/types.hpp"

namespace kafka {

// Numeric codes that represent different types of requests.
enum class ApiKey : INT16 {
    FETCH = 1,
    API_VERSIONS = 18,
    DESCRIBE_TOPIC_PARTITIONS = 75,
};

// Numeric codes that indicate what problem occurred on the server.
enum class ErrorCode : INT16 {
    NONE = 0,
    // This server does not host this topic-partition.
    UNKNOWN_TOPIC_OR_PARTITION = 3,
    // The version of API is not supported.
    UNSUPPORTED_VERSION = 35,
    // This server does not host this topic ID.
    UNKNOWN_TOPIC_ID = 100,
};

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_CONSTANTS_HPP_INCLUDED
