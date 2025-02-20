#ifndef codecrafters_kafka_utils_bytes_io_hpp
#define codecrafters_kafka_utils_bytes_io_hpp

#include <cstddef>
#include <vector>

#include "kafka/protocol/readable.hpp"
#include "kafka/protocol/writable.hpp"

namespace kafka {

// In-memory byte stream.
class BytesIO : public Readable, public Writable {
public:
        BytesIO() : pos_(0) {}
        explicit BytesIO(int fildes, std::size_t size);

        void read(void *buffer, std::size_t size) override;
        void write(const void *buffer, std::size_t size) override;

        // Returns the entire contents of the underlying buffer.
        const std::vector<unsigned char> &get_value() const {
                return data_;
        }

private:
        std::vector<unsigned char> data_;
        std::size_t pos_;

        // Returns the number of remaining bytes in this byte stream.
        std::size_t remaining() const {
                return data_.size() - pos_;
        }
};

}

#endif
