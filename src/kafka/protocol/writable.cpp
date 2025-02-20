#include "kafka/protocol/writable.hpp"

#include <bit>
#include <cstdint>

template<typename IntType>
static IntType host_to_network(IntType n) {
        return std::endian::native == std::endian::big ? n : std::byteswap(n);
}

namespace kafka {

void write_int16(Writable &writable, std::int16_t n) {
        n = host_to_network(n);
        writable.write(&n, sizeof(n));
}

void write_int32(Writable &writable, std::int32_t n) {
        n = host_to_network(n);
        writable.write(&n, sizeof(n));
}

void write_unsigned_varint(Writable &writable, std::uint32_t n) {
        do {
                unsigned char c = n & 0x7F;
                if ( (n >>= 7) > 0) {
                        c |= 0x80;
                }
                writable.write(&c, sizeof(c));
        } while (n > 0);
}

void write_tagged_fields(Writable &writable) {
        const unsigned char c = 0x00;
        writable.write(&c, sizeof(c));
}

}
