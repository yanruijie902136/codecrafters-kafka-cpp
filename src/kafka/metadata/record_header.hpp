#ifndef codecrafters_kafka_metadata_record_header_hpp
#define codecrafters_kafka_metadata_record_header_hpp

#include "kafka/protocol/readable.hpp"

namespace kafka {

class RecordHeader {
public:
        void read(Readable &readable) {
                header_key_length_ = read_int32(readable);
                header_key_.resize(header_key_length_);
                readable.read(header_key_.data(), header_key_length_);
                header_value_length_ = read_varint(readable);
                value_.resize(header_value_length_);
                readable.read(value_.data(), header_value_length_);
        }

private:
        std::int32_t header_key_length_;
        std::string header_key_;
        std::int32_t header_value_length_;
        std::vector<unsigned char> value_;
};

}

#endif
