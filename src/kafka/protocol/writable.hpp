#ifndef codecrafters_kafka_protocol_writable_hpp
#define codecrafters_kafka_protocol_writable_hpp

#include <cstddef>
#include <cstdint>
#include <vector>

namespace kafka {

// Interface of a writable byte stream.
class Writable {
public:
        virtual ~Writable() {}

        // Writes bytes to this byte stream.
        virtual void write(const void *buffer, std::size_t size) = 0;
};

// Writes an INT16 to a byte stream.
void write_int16(Writable &writable, std::int16_t n);

// Writes an INT32 to a byte stream.
void write_int32(Writable &writable, std::int32_t n);

// Writes an UNSIGNED_VARINT to a byte stream.
void write_unsigned_varint(Writable &writable, std::uint32_t n);

// Writes a COMPACT_ARRAY to a byte stream.
template<typename T>
inline void write_compact_array(Writable &writable, const std::vector<T> &arr) {
        write_unsigned_varint(writable, arr.size() + 1);
        for (const T &object : arr) {
                object.write(writable);
        }
}

// Writes tagged fields to a byte stream.
void write_tagged_fields(Writable &writable);

}

#endif
