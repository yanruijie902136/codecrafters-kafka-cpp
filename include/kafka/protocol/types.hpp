#ifndef CODECRAFTERS_KAFKA_PROTOCOL_TYPES_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_TYPES_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <vector>

namespace kafka {

using BOOLEAN = bool;
using INT8 = std::int8_t;
using INT16 = std::int16_t;
using INT32 = std::int32_t;
using INT64 = std::int64_t;
using UINT32 = std::uint32_t;
using VARINT = std::int32_t;
using UNSIGNED_VARINT = std::uint32_t;
using VARLONG = std::int64_t;
using UNSIGNED_VARLONG = std::uint64_t;
using COMPACT_STRING = std::string;
using NULLABLE_STRING = std::string;
using COMPACT_NULLABLE_STRING = std::string;
using BYTES = std::vector<unsigned char>;
template<typename T>
using ARRAY = std::vector<T>;
template<typename T>
using COMPACT_ARRAY = std::vector<T>;

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_TYPES_HPP_INCLUDED
