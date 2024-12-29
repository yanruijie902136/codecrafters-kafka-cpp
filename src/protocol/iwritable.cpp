#include "kafka/protocol/iwritable.hpp"

#include <arpa/inet.h>
#include <utility>

namespace kafka {

void write_boolean(IWritable &writable, BOOLEAN boolean) {
    char c = boolean ? 0x01 : 0x00;
    writable.write(&c, sizeof(c));
}

void write_int16(IWritable &writable, INT16 n) {
    n = htons(n);
    writable.write(&n, sizeof(n));
}

void write_int32(IWritable &writable, INT32 n) {
    n = htonl(n);
    writable.write(&n, sizeof(n));
}

void write_unsigned_varint(IWritable &writable, UNSIGNED_VARINT n) {
    do {
        char c = n & 0x7F;
        if ((n >>= 7) > 0) {
            c |= 0x80;
        }
        writable.write(&c, sizeof(c));
    } while (n > 0);
}

void write_uuid(IWritable &writable, const UUID &uuid) {
    writable.write(uuid.data(), uuid.size());
}

void write_compact_nullable_string(IWritable &writable, const COMPACT_NULLABLE_STRING &str) {
    write_unsigned_varint(writable, str.size() + 1);
    writable.write(str.data(), str.size());
}

void write_bytes(IWritable &writable, const BYTES &bytes) {
    write_int32(writable, bytes.size());
    writable.write(bytes.data(), bytes.size());
}

void write_tagged_fields(IWritable &writable) {
    static constexpr char c = 0x00;
    writable.write(&c, sizeof(c));
}

void write_api_key(IWritable &writable, ApiKey api_key) {
    write_int16(writable, std::to_underlying(api_key));
}

void write_error_code(IWritable &writable, ErrorCode error_code) {
    write_int16(writable, std::to_underlying(error_code));
}

}
