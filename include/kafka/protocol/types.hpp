#ifndef CODECRAFTERS_KAFKA_PROTOCOL_TYPES_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_TYPES_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <vector>

namespace kafka {

using INT16 = std::int16_t;
using INT32 = std::int32_t;
using UNSIGNED_VARINT = std::uint32_t;
using COMPACT_STRING = std::string;
using NULLABLE_STRING = std::string;
using BYTES = std::vector<char>;
template<typename T>
using COMPACT_ARRAY = std::vector<T>;

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_TYPES_HPP_INCLUDED
