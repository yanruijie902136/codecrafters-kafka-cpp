#ifndef CODECRAFTERS_KAFKA_PROTOCOL_WRITABLE_BUFFER_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_WRITABLE_BUFFER_HPP_INCLUDED

#include "kafka/protocol/iwritable.hpp"
#include "kafka/protocol/types.hpp"

namespace kafka {

// Writable in-memory byte buffer.
class WritableBuffer : public IWritable {
public:
    // Writes a specified number of bytes to this byte buffer.
    void write(const void *src, std::size_t nbytes) override {
        const char *p = static_cast<const char *>(src);
        bytes_.insert(bytes_.end(), p, p + nbytes);
    }

    // Returns the underlying byte buffer.
    const BYTES &buffer() const {
        return bytes_;
    }

private:
    BYTES bytes_;
};

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_WRITABLE_BUFFER_HPP_INCLUDED
