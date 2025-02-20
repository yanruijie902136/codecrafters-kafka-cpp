#ifndef codecrafters_kafka_protocol_writable_hpp
#define codecrafters_kafka_protocol_writable_hpp

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace kafka {

// Interface of a writable byte stream.
class Writable {
public:
        virtual ~Writable() {}

        // Writes bytes to this byte stream.
        virtual void write(const void *buffer, std::size_t size) = 0;
};

template<typename T>
using WriteObjectFunction = std::function<void(Writable &, const T &)>;

// Writes a BOOLEAN to a byte stream.
void write_boolean(Writable &writable, bool b);

// Writes an INT8 to a byte stream.
void write_int8(Writable &writable, std::int8_t n);

// Writes an INT16 to a byte stream.
void write_int16(Writable &writable, std::int16_t n);

// Writes an INT32 to a byte stream.
void write_int32(Writable &writable, std::int32_t n);

// Writes an INT64 to a byte stream.
void write_int64(Writable &writable, std::int64_t n);

// Writes an UINT32 to a byte stream.
void write_uint32(Writable &writable, std::uint32_t n);

// Writes an UNSIGNED_VARINT to a byte stream.
void write_unsigned_varint(Writable &writable, std::uint32_t n);

// Writes a VARINT to a byte stream.
void write_varint(Writable &writable, std::int32_t n);

// Writes an UNSIGNED_VARLONG to a byte stream.
void write_unsigned_varlong(Writable &writable, std::uint64_t n);

// Writes a VARLONG to a byte stream.
void write_varlong(Writable &writable, std::int64_t n);

// Writes a COMPACT_NULLABLE_STRING to a byte stream.
void write_compact_nullable_string(Writable &writable, const std::string &s);

// Writes an ARRAY to a byte stream.
template<typename T>
inline void write_array(Writable &writable, const std::vector<T> &arr) {
        write_int32(writable, arr.size());
        for (const T &object : arr) {
                object.write(writable);
        }
}

// Writes a COMPACT_ARRAY to a byte stream.
template<typename T>
inline void write_compact_array(Writable &writable, const std::vector<T> &arr) {
        write_unsigned_varint(writable, arr.size() + 1);
        for (const T &object : arr) {
                object.write(writable);
        }
}

// Writes a COMPACT_ARRAY to a byte stream.
template<typename T>
inline void write_compact_array(Writable &writable, const std::vector<T> &arr, const WriteObjectFunction<T> &write_function) {
        write_unsigned_varint(writable, arr.size() + 1);
        for (const T &object : arr) {
                write_function(writable, object);
        }
}

// Writes tagged fields to a byte stream.
void write_tagged_fields(Writable &writable);

}

#endif
