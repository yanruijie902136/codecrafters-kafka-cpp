#include "kafka/protocol/ireadable.hpp"
#include "kafka/utils.hpp"

namespace kafka {

INT8 read_int8(IReadable &readable) {
    INT8 n;
    readable.read(&n, sizeof(n));
    return n;
}

INT16 read_int16(IReadable &readable) {
    INT16 n;
    readable.read(&n, sizeof(n));
    return to_host_byte_order(n);
}

INT32 read_int32(IReadable &readable) {
    return static_cast<INT32>(read_uint32(readable));
}

INT64 read_int64(IReadable &readable) {
    INT64 n;
    readable.read(&n, sizeof(n));
    return to_host_byte_order(n);
}

UINT32 read_uint32(IReadable &readable) {
    UINT32 n;
    readable.read(&n, sizeof(n));
    return to_host_byte_order(n);
}

VARINT read_varint(IReadable &readable) {
    return static_cast<VARINT>(read_varlong(readable));
}

UNSIGNED_VARINT read_unsigned_varint(IReadable &readable) {
    return static_cast<UNSIGNED_VARINT>(read_unsigned_varlong(readable));
}

VARLONG read_varlong(IReadable &readable) {
    UNSIGNED_VARLONG n = read_unsigned_varlong(readable);
    return (n & 1) ? -((n + 1) >> 1) : (n >> 1);
}

UNSIGNED_VARLONG read_unsigned_varlong(IReadable &readable) {
    UNSIGNED_VARLONG n = 0;
    for (unsigned char c, i = 0; ; i += 7) {
        readable.read(&c, sizeof(c));
        n += static_cast<UNSIGNED_VARLONG>(c & 0x7F) << i;
        if (!(c & 0x80)) {
            return n;
        }
    }
}

UUID read_uuid(IReadable &readable) {
    UUID uuid;
    readable.read(uuid.data(), uuid.size());
    return uuid;
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
        throw_runtime_error("unexpected tagged fields");
    }
}

ApiKey read_api_key(IReadable &readable) {
    return static_cast<ApiKey>(read_int16(readable));
}

}
