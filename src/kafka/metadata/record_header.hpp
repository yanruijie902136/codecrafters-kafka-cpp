#ifndef codecrafters_kafka_metadata_record_header_hpp
#define codecrafters_kafka_metadata_record_header_hpp

#include "kafka/protocol/readable.hpp"
#include "kafka/protocol/writable.hpp"

namespace kafka {

class RecordHeader {
public:
        void read(Readable &readable) {
                std::int32_t header_key_length = read_varint(readable);
                header_key_.resize(header_key_length);
                readable.read(header_key_.data(), header_key_length);

                std::int32_t header_value_length = read_varint(readable);
                value_.resize(header_value_length);
                readable.read(value_.data(), header_value_length);
        }

        void write(Writable &writable) const {
                write_varint(writable, header_key_.size());
                writable.write(header_key_.data(), header_key_.size());

                write_varint(writable, value_.size());
                writable.write(value_.data(), value_.size());
        }

private:
        std::string header_key_;
        std::vector<unsigned char> value_;
};

}

#endif
