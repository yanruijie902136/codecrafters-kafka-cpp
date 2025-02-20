#ifndef codecrafters_kafka_metadata_record_hpp
#define codecrafters_kafka_metadata_record_hpp

#include <cstdint>
#include <vector>

#include "kafka/metadata/record_header.hpp"
#include "kafka/protocol/readable.hpp"

namespace kafka {

class Record {
public:
        void read(Readable &readable) {
                length_ = read_varint(readable);
                attributes_ = read_int8(readable);
                timestamp_delta_ = read_varlong(readable);
                offset_delta_ = read_varint(readable);
                read_key(readable);
                read_value(readable);
                headers_ = read_compact_array<RecordHeader>(readable);
        }

        const std::vector<unsigned char> &value() const {
                return value_;
        }

private:
        std::int32_t length_;
        std::int8_t attributes_;
        std::int64_t timestamp_delta_;
        std::int32_t offset_delta_;
        std::int32_t key_length_;
        std::vector<unsigned char> key_;
        std::int32_t value_len_;
        std::vector<unsigned char> value_;
        std::vector<RecordHeader> headers_;

        void read_key(Readable &readable) {
                key_length_ = read_varint(readable);
                if (key_length_ < 0) {
                        key_.clear();
                } else {
                        key_.resize(key_length_);
                        readable.read(key_.data(), key_length_);
                }
        }

        void read_value(Readable &readable) {
                value_len_ = read_varint(readable);
                value_.resize(value_len_);
                readable.read(value_.data(), value_len_);
        }
};

}

#endif
