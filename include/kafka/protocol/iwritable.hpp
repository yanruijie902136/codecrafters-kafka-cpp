#ifndef CODECRAFTERS_KAFKA_PROTOCOL_IWRITABLE_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_IWRITABLE_HPP_INCLUDED

#include <cstddef>
#include <functional>

#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/protocol/uuid.hpp"

namespace kafka {

// Interface of a writable byte stream.
class IWritable {
public:
    virtual ~IWritable() = default;

    // Writes a specified number of bytes to this byte stream.
    virtual void write(const void *src, std::size_t nbytes) = 0;
};

// Writes a BOOLEAN to a byte stream.
void write_boolean(IWritable &writable, BOOLEAN boolean);

// Writes an INT8 to a byte stream.
void write_int8(IWritable &writable, INT8 n);

// Writes an INT16 to a byte stream.
void write_int16(IWritable &writable, INT16 n);

// Writes an INT32 to a byte stream.
void write_int32(IWritable &writable, INT32 n);

// Writes an INT64 to a byte stream.
void write_int64(IWritable &writable, INT64 n);

// Writes an UINT32 to a byte stream.
void write_uint32(IWritable &writable, UINT32 n);

// Writes a VARINT to a byte stream.
void write_varint(IWritable &writable, VARINT n);

// Writes an UNSIGNED_VARINT to a byte stream.
void write_unsigned_varint(IWritable &writable, UNSIGNED_VARINT n);

// Writes a VARLONG to a byte stream.
void write_varlong(IWritable &writable, VARLONG n);

// Writes an UNSIGNED_VARLONG to a byte stream.
void write_unsigned_varlong(IWritable &writable, UNSIGNED_VARLONG n);

// Writes a UUID to a byte stream.
void write_uuid(IWritable &writable, const UUID &uuid);

// Writes a COMPACT_NULLABLE_STRING to a byte stream.
void write_compact_nullable_string(IWritable &writable, const COMPACT_NULLABLE_STRING &str);

// Writes a BYTES to a byte stream.
void write_bytes(IWritable &writable, const BYTES &bytes);

// Writes an ARRAY to a byte stream.
template<typename T>
inline void write_array(IWritable &writable, const ARRAY<T> &arr) {
    write_int32(writable, arr.size());
    for (const T &object : arr) {
        object.write(writable);
    }
}

// Writes a COMPACT_ARRAY to a byte stream.
template<typename T>
inline void write_compact_array(IWritable &writable, const COMPACT_ARRAY<T> &arr) {
    write_unsigned_varint(writable, arr.size() + 1);
    for (const T &object : arr) {
        object.write(writable);
    }
}

template<typename T>
using WriteObjectFunction = std::function<void(IWritable &, const T &)>;

// Writes a COMPACT_ARRAY to a byte stream.
template<typename T>
inline void write_compact_array(IWritable &writable, const COMPACT_ARRAY<T> &arr, const WriteObjectFunction<T> &write_function) {
    write_unsigned_varint(writable, arr.size() + 1);
    for (const T &object : arr) {
        write_function(writable, object);
    }
}

// Writes tagged fields to a byte stream.
void write_tagged_fields(IWritable &writable);

// Writes an `ApiKey` to a byte stream.
void write_api_key(IWritable &writable, ApiKey api_key);

// Writes an `ErrorCode` to a byte stream.
void write_error_code(IWritable &writable, ErrorCode error_code);

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_IWRITABLE_HPP_INCLUDED
