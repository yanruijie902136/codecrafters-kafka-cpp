#ifndef CODECRAFTERS_KAFKA_PROTOCOL_READABLE_BUFFER_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_READABLE_BUFFER_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

#include "kafka/protocol/ireadable.hpp"

namespace kafka {

// Readable in-memory byte buffer.
class ReadableBuffer : public IReadable {
public:
    explicit ReadableBuffer(BYTES bytes) : bytes_(std::move(bytes)), index_(0) {}

    // Reads a specified number of bytes from this byte buffer.
    void read(void *dst, std::size_t nbytes) override {
        auto first = bytes_.begin() + index_;
        auto last = first + nbytes;
        if (last > bytes_.end()) {
            throw std::runtime_error("ReadableBuffer underflow");
        }
        std::copy(first, last, static_cast<char *>(dst));
        index_ += nbytes;
    }

private:
    BYTES bytes_;
    std::size_t index_;
};

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_READABLE_BUFFER_HPP_INCLUDED