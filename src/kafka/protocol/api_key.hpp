#ifndef codecrafters_kafka_protocol_api_key_hpp
#define codecrafters_kafka_protocol_api_key_hpp

#include <cstdint>

#include "kafka/protocol/readable.hpp"
#include "kafka/protocol/writable.hpp"

namespace kafka {

enum class ApiKey : std::int16_t {
        API_VERSIONS = 18,
};

// Reads an ApiKey from a byte stream.
ApiKey read_api_key(Readable &readable);

// Writes an ApiKey to a byte stream.
void write_api_key(Writable &writable, ApiKey api_key);

}

#endif
