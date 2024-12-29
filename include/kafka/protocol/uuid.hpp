#ifndef CODECRAFTERS_KAFKA_PROTOCOL_UUID_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_UUID_HPP_INCLUDED

#include <cstddef>
#include <cstring>

namespace kafka {

// Type 4 immutable universally unique identifier.
class UUID {
public:
    UUID() {
        std::memset(data_, 0, sizeof(data_));
    }

    unsigned char *data() {
        return data_;
    }

    const unsigned char *data() const {
        return data_;
    }

    constexpr std::size_t size() const {
        return sizeof(data_);
    }

private:
    unsigned char data_[16];
};

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_UUID_HPP_INCLUDED
