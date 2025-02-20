#include "kafka/protocol/readable.hpp"

#include <bit>
#include <cstdint>
#include <stdexcept>
#include <string>

template<typename IntType>
static IntType network_to_host(IntType n) {
        return std::endian::native == std::endian::big ? n : std::byteswap(n);
}

namespace kafka {

std::int8_t read_int8(Readable &readable) {
        std::int8_t n;
        readable.read(&n, sizeof(n));
        return n;
}

std::int16_t read_int16(Readable &readable) {
        std::int16_t n;
        readable.read(&n, sizeof(n));
        return network_to_host(n);
}

std::int32_t read_int32(Readable &readable) {
        std::int32_t n;
        readable.read(&n, sizeof(n));
        return network_to_host(n);
}

std::int64_t read_int64(Readable &readable) {
        std::int64_t n;
        readable.read(&n, sizeof(n));
        return network_to_host(n);
}

std::uint32_t read_uint32(Readable &readable) {
        std::uint32_t n;
        readable.read(&n, sizeof(n));
        return network_to_host(n);
}

std::uint32_t read_unsigned_varint(Readable &readable) {
        return static_cast<std::uint32_t>(read_unsigned_varlong(readable));
}

std::int32_t read_varint(Readable &readable) {
        return static_cast<std::int32_t>(read_varlong(readable));
}

std::uint64_t read_unsigned_varlong(Readable &readable) {
        std::uint64_t n = 0;
        for (std::uint64_t i = 0; ; i += 7) {
                unsigned char c;
                readable.read(&c, sizeof(c));
                n += (c & 0x7F) << i;
                if (!(c & 0x80)) {
                        return n;
                }
        }
}

std::int64_t read_varlong(Readable &readable) {
        std::uint64_t n = read_unsigned_varlong(readable);
        return (n & 1) ? -((n >> 1) + 1) : (n >> 1);
}

std::string read_compact_string(Readable &readable) {
        std::uint32_t n = read_unsigned_varint(readable);
        if (n == 0) {
                throw std::runtime_error("incorrect compact string format");
        }
        std::string s(--n, 0);
        readable.read(s.data(), n);
        return s;
}

std::string read_nullable_string(Readable &readable) {
        std::int16_t n = read_int16(readable);
        if (n < 0) {
                return "";
        }
        std::string s(n, 0);
        readable.read(s.data(), n);
        return s;
}

void read_tagged_fields(Readable &readable) {
        unsigned char c;
        readable.read(&c, sizeof(c));
        if (c != 0x00) {
                throw std::runtime_error("unexpected tagged fields");
        }
}

}
