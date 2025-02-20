#include "kafka/utils/bytes_io.hpp"
#include "kafka/utils/syscalls.hpp"

#include <algorithm>
#include <stdexcept>

namespace kafka {

BytesIO::BytesIO(int fildes, std::size_t size) : data_(size), pos_(0) {
        fullread(fildes, data_.data(), size);
}

void BytesIO::read(void *buffer, std::size_t size) {
        if (remaining() < size) {
                throw std::runtime_error("buffer underflow");
        }
        const auto first = data_.begin() + pos_;
        const auto last = first + size;
        std::copy(first, last, static_cast<unsigned char *>(buffer));
        pos_ += size;
}

void BytesIO::write(const void *buffer, std::size_t size) {
        if (remaining() < size) {
                data_.resize(pos_ + size);
        }
        const unsigned char *first = static_cast<const unsigned char *>(buffer);
        const unsigned char *last = first + size;
        std::copy(first, last, data_.begin() + pos_);
        pos_ += size;
}

}
