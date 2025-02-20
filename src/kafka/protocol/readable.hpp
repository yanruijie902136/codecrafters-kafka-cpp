#ifndef codecrafters_kafka_protocol_readable_hpp
#define codecrafters_kafka_protocol_readable_hpp

#include <cstddef>
#include <cstdint>
#include <string>

namespace kafka {

// Interface of a readable byte stream.
class Readable {
public:
        virtual ~Readable() {}

        // Reads bytes from this byte stream.
        virtual void read(void *buffer, std::size_t size) = 0;
};

// Reads an INT16 from a byte stream.
std::int16_t read_int16(Readable &readable);

// Reads an INT32 from a byte stream.
std::int32_t read_int32(Readable &readable);

// Reads an UNSIGNED_VARINT from a byte stream.
std::uint32_t read_unsigned_varint(Readable &readable);

// Reads a COMPACT_STRING from a byte stream.
std::string read_compact_string(Readable &readable);

// Reads a NULLABLE_STRING from a byte stream.
std::string read_nullable_string(Readable &readable);

// Reads tagged fields from a byte stream.
void read_tagged_fields(Readable &readable);

}

#endif
