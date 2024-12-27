#ifndef CODECRAFTERS_KAFKA_PROTOCOL_IWRITABLE_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_IWRITABLE_HPP_INCLUDED

#include <cstddef>

#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/types.hpp"

namespace kafka {

// Interface of a writable byte stream.
class IWritable {
public:
    virtual ~IWritable() = default;

    // Writes a specified number of bytes to this byte stream.
    virtual void write(const void *src, std::size_t nbytes) = 0;
};

// Writes an INT16 to a byte stream.
void write_int16(IWritable &writable, INT16 n);

// Writes an INT32 to a byte stream.
void write_int32(IWritable &writable, INT32 n);

// Writes an UNSIGNED_VARINT to a byte stream.
void write_unsigned_varint(IWritable &writable, UNSIGNED_VARINT n);

// Writes a BYTES to a byte stream.
void write_bytes(IWritable &writable, const BYTES &bytes);

// Writes a COMPACT_ARRAY to a byte stream.
template<typename T>
inline void write_compact_array(IWritable &writable, const COMPACT_ARRAY<T> &arr) {
    write_unsigned_varint(writable, arr.size() + 1);
    for (const T &object : arr) {
        object.write(writable);
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
