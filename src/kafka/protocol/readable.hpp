#ifndef codecrafters_kafka_protocol_readable_hpp
#define codecrafters_kafka_protocol_readable_hpp

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace kafka {

// Interface of a readable byte stream.
class Readable {
public:
        virtual ~Readable() {}

        // Reads bytes from this byte stream.
        virtual void read(void *buffer, std::size_t size) = 0;
};

template<typename T>
using ReadObjectFunction = std::function<T(Readable &)>;

// Reads an INT8 from a byte stream.
std::int8_t read_int8(Readable &readable);

// Reads an INT16 from a byte stream.
std::int16_t read_int16(Readable &readable);

// Reads an INT32 from a byte stream.
std::int32_t read_int32(Readable &readable);

// Reads an INT64 from a byte stream.
std::int64_t read_int64(Readable &readable);

// Reads an UINT32 from a byte stream.
std::uint32_t read_uint32(Readable &readable);

// Reads an UNSIGNED_VARINT from a byte stream.
std::uint32_t read_unsigned_varint(Readable &readable);

// Reads a VARINT from a byte stream.
std::int32_t read_varint(Readable &readable);

// Reads an UNSIGNED_VARLONG from a byte stream.
std::uint64_t read_unsigned_varlong(Readable &readable);

// Reads a VARLONG from a byte stream.
std::int64_t read_varlong(Readable &readable);

// Reads a COMPACT_STRING from a byte stream.
std::string read_compact_string(Readable &readable);

// Reads a NULLABLE_STRING from a byte stream.
std::string read_nullable_string(Readable &readable);

// Reads an ARRAY from a byte stream.
template<typename T>
inline std::vector<T> read_array(Readable &readable) {
        std::int32_t n = read_int32(readable);
        if (n < 0) {
                return {};
        }
        std::vector<T> arr(n);
        for (T &object : arr) {
                object.read(readable);
        }
        return arr;
}

// Reads a COMPACT_ARRAY from a byte stream.
template<typename T>
inline std::vector<T> read_compact_array(Readable &readable) {
        std::uint32_t n = read_unsigned_varint(readable);
        if (n == 0) {
                return {};
        }
        std::vector<T> arr(--n);
        for (T &object : arr) {
                object.read(readable);
        }
        return arr;
}

// Reads a COMPACT_ARRAY from a byte stream.
template<typename T>
inline std::vector<T> read_compact_array(Readable &readable, ReadObjectFunction<T> read_function) {
        std::uint32_t n = read_unsigned_varint(readable);
        if (n == 0) {
                return {};
        }
        std::vector<T> arr;
        for (arr.reserve(--n); n > 0; n--) {
                arr.push_back(read_function(readable));
        }
        return arr;
}

// Reads tagged fields from a byte stream.
void read_tagged_fields(Readable &readable);

}

#endif
