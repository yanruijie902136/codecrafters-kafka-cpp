#ifndef CODECRAFTERS_KAFKA_PROTOCOL_CONSTANTS_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_CONSTANTS_HPP_INCLUDED

#include "kafka/protocol/types.hpp"

namespace kafka {

// Numeric codes that represent different types of requests.
enum class ApiKey : INT16 {
    API_VERSIONS = 18,
};

// Numeric codes that indicate what problem occurred on the server.
enum class ErrorCode : INT16 {
    NONE = 0,
    // The version of API is not supported.
    UNSUPPORTED_VERSION = 35,
};

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_CONSTANTS_HPP_INCLUDED
