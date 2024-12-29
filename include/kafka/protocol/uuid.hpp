#ifndef CODECRAFTERS_KAFKA_PROTOCOL_UUID_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_UUID_HPP_INCLUDED

#include <cstddef>
#include <cstring>
#include <ostream>

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

    friend std::ostream &operator<<(std::ostream &os, const UUID &uuid) {
        for (std::size_t i = 0; i < uuid.size(); i++) {
            os << std::hex << (int)uuid.data()[i];
        }
        os << std::dec;
        return os;
    }

private:
    unsigned char data_[16];
};

// Utility class that allows UUID to be stored as key in std::map.
struct UUIDCompare {
    bool operator()(const UUID &uuid1, const UUID &uuid2) const {
        return std::memcmp(uuid1.data(), uuid2.data(), uuid1.size()) < 0;
    }
};

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_UUID_HPP_INCLUDED
