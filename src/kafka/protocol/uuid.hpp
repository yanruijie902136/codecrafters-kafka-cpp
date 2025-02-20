#ifndef codecrafters_kafka_protocol_uuid_hpp
#define codecrafters_kafka_protocol_uuid_hpp

#include <cstring>

#include "kafka/protocol/writable.hpp"

namespace kafka {

// Universally Unique Identifier.
class Uuid {
public:
        Uuid() {
                std::memset(data_, 0, sizeof(data_));
        }

        void write(Writable &writable) const {
                writable.write(data_, sizeof(data_));
        }

private:
        unsigned char data_[16];
};

}

#endif
