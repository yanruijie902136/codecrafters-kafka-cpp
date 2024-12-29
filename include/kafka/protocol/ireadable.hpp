#ifndef CODECRAFTERS_KAFKA_PROTOCOL_IREADABLE_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_IREADABLE_HPP_INCLUDED

#include <cstddef>
#include <functional>

#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/protocol/uuid.hpp"

namespace kafka {

// Interface of a readable byte stream.
class IReadable {
public:
    virtual ~IReadable() = default;

    // Reads a specified number of bytes from this byte stream.
    virtual void read(void *dst, std::size_t nbytes) = 0;
};

// Reads an INT8 from a byte stream.
INT8 read_int8(IReadable &readable);

// Reads an INT16 from a byte stream.
INT16 read_int16(IReadable &readable);

// Reads an INT32 from a byte stream.
INT32 read_int32(IReadable &readable);

// Reads an INT64 from a byte stream.
INT64 read_int64(IReadable &readable);

// Reads an UINT32 from a byte stream.
UINT32 read_uint32(IReadable &readable);

// Reads a VARINT from a byte stream.
VARINT read_varint(IReadable &readable);

// Reads an UNSIGNED_VARINT from a byte stream.
UNSIGNED_VARINT read_unsigned_varint(IReadable &readable);

// Reads a VARLONG from a byte stream.
VARLONG read_varlong(IReadable &readable);

// Reads an UNSIGNED_VARLONG from a byte stream.
UNSIGNED_VARLONG read_unsigned_varlong(IReadable &readable);

// Reads a UUID from a byte stream.
UUID read_uuid(IReadable &readable);

// Reads a COMPACT_STRING from a byte stream.
COMPACT_STRING read_compact_string(IReadable &readable);

// Reads a NULLABLE_STRING from a byte stream.
NULLABLE_STRING read_nullable_string(IReadable &readable);

// Reads a BYTES from a byte stream.
BYTES read_bytes(IReadable &readable);

// Reads an ARRAY from a byte stream.
template<typename T>
inline ARRAY<T> read_array(IReadable &readable) {
    INT32 n = read_int32(readable);
    if (n < 0) {
        return {};
    }
    ARRAY<T> arr(n);
    for (T &object : arr) {
        object.read(readable);
    }
    return arr;
}

// Reads a COMPACT_ARRAY from a byte stream.
template<typename T>
inline COMPACT_ARRAY<T> read_compact_array(IReadable &readable) {
    UNSIGNED_VARINT n = read_unsigned_varint(readable);
    if (n == 0) {
        return {};
    }
    COMPACT_ARRAY<T> arr(--n);
    for (T &object : arr) {
        object.read(readable);
    }
    return arr;
}

template<typename T>
using ReadObjectFunction = std::function<T(IReadable &)>;

// Reads a COMPACT_ARRAY from a byte stream.
template<typename T>
inline COMPACT_ARRAY<T> read_compact_array(IReadable &readable, const ReadObjectFunction<T> &read_function) {
    UNSIGNED_VARINT n = read_unsigned_varint(readable);
    if (n == 0) {
        return {};
    }
    COMPACT_ARRAY<T> arr;
    arr.reserve(--n);
    while (n--) {
        arr.push_back(read_function(readable));
    }
    return arr;
}

// Reads tagged fields from a byte stream.
void read_tagged_fields(IReadable &readable);

// Reads an `ApiKey` from a byte stream.
ApiKey read_api_key(IReadable &readable);

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_IREADABLE_HPP_INCLUDED
