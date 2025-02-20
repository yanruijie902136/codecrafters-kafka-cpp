#include "kafka/protocol/writable.hpp"

#include <bit>
#include <cstdint>

template<typename IntType>
static IntType host_to_network(IntType n) {
        return std::endian::native == std::endian::big ? n : std::byteswap(n);
}

namespace kafka {

void write_boolean(Writable &writable, bool b) {
        unsigned char c = b ? 0x01 : 0x00;
        writable.write(&c, sizeof(c));
}

void write_int8(Writable &writable, std::int8_t n) {
        writable.write(&n, sizeof(n));
}

void write_int16(Writable &writable, std::int16_t n) {
        n = host_to_network(n);
        writable.write(&n, sizeof(n));
}

void write_int32(Writable &writable, std::int32_t n) {
        n = host_to_network(n);
        writable.write(&n, sizeof(n));
}

void write_int64(Writable &writable, std::int64_t n) {
        n = host_to_network(n);
        writable.write(&n, sizeof(n));
}

void write_uint32(Writable &writable, std::uint32_t n) {
        n = host_to_network(n);
        writable.write(&n, sizeof(n));
}

void write_unsigned_varint(Writable &writable, std::uint32_t n) {
        write_unsigned_varlong(writable, n);
}

void write_varint(Writable &writable, std::int32_t n) {
        write_unsigned_varint(writable, (n << 1) ^ (n >> 31));
}

void write_unsigned_varlong(Writable &writable, std::uint64_t n) {
        do {
                unsigned char c = n & 0x7F;
                if ( (n >>= 7) > 0) {
                        c |= 0x80;
                }
                writable.write(&c, sizeof(c));
        } while (n > 0);
}

void write_varlong(Writable &writable, std::int64_t n) {
        write_unsigned_varlong(writable, (n << 1) ^ (n >> 63));
}

void write_compact_nullable_string(Writable &writable, const std::string &s) {
        write_unsigned_varint(writable, s.size() + 1);
        writable.write(s.data(), s.size());
}

void write_tagged_fields(Writable &writable) {
        const unsigned char c = 0x00;
        writable.write(&c, sizeof(c));
}

}
