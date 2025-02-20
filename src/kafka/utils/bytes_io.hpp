#ifndef codecrafters_kafka_utils_bytes_io_hpp
#define codecrafters_kafka_utils_bytes_io_hpp

#include <cstddef>
#include <utility>
#include <vector>

#include "kafka/protocol/readable.hpp"
#include "kafka/protocol/writable.hpp"

namespace kafka {

// In-memory byte stream.
class BytesIO : public Readable, public Writable {
public:
        BytesIO() : pos_(0) {}
        explicit BytesIO(std::vector<unsigned char> data) : data_(std::move(data)), pos_(0) {}

        // Initialize with a file's entire contents.
        explicit BytesIO(const char *path);

        // Initialize with `size` bytes read from a file descriptor.
        BytesIO(int fildes, std::size_t size);

        void read(void *buffer, std::size_t size) override;
        void write(const void *buffer, std::size_t size) override;

        // Returns the entire contents of the underlying buffer.
        const std::vector<unsigned char> &get_value() const {
                return data_;
        }

        // Returns the number of remaining bytes in this byte stream.
        std::size_t remaining() const {
                return data_.size() - pos_;
        }

private:
        std::vector<unsigned char> data_;
        std::size_t pos_;
};

}

#endif
