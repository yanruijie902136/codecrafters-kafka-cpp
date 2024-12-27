#include "kafka/protocol/ireadable.hpp"

#include <arpa/inet.h>
#include <stdexcept>

namespace kafka {

INT16 read_int16(IReadable &readable) {
    INT16 n;
    readable.read(&n, sizeof(n));
    return htons(n);
}

INT32 read_int32(IReadable &readable) {
    INT32 n;
    readable.read(&n, sizeof(n));
    return htonl(n);
}

UNSIGNED_VARINT read_unsigned_varint(IReadable &readable) {
    UNSIGNED_VARINT n = 0;
    for (char c, i = 0; ; i += 7) {
        readable.read(&c, sizeof(c));
        n += static_cast<UNSIGNED_VARINT>(c & 0x7F) << i;
        if (~(c & 0x80)) {
            return n;
        }
    }
}

COMPACT_STRING read_compact_string(IReadable &readable) {
    UNSIGNED_VARINT n = read_unsigned_varint(readable);
    if (n == 0) {
        return "";
    }
    COMPACT_STRING str(--n, 0);
    readable.read(str.data(), str.size());
    return str;
}

NULLABLE_STRING read_nullable_string(IReadable &readable) {
    INT16 n = read_int16(readable);
    if (n < 0) {
        return "";
    }
    NULLABLE_STRING str(n, 0);
    readable.read(str.data(), str.size());
    return str;
}

BYTES read_bytes(IReadable &readable) {
    INT32 n = read_int32(readable);
    BYTES bytes(n);
    readable.read(bytes.data(), bytes.size());
    return bytes;
}

void read_tagged_fields(IReadable &readable) {
    char c;
    readable.read(&c, sizeof(c));
    if (c != 0x00) {
        throw std::runtime_error("unexpected tagged fields");
    }
}

ApiKey read_api_key(IReadable &readable) {
    return static_cast<ApiKey>(read_int16(readable));
}

}
