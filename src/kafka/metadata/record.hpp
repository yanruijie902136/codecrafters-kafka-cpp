#ifndef codecrafters_kafka_metadata_record_hpp
#define codecrafters_kafka_metadata_record_hpp

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "kafka/protocol/readable.hpp"
#include "kafka/protocol/writable.hpp"

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
                read_headers(readable);
        }

        void write(Writable &writable) const {
                write_varint(writable, length_);
                write_int8(writable, attributes_);
                write_varlong(writable, timestamp_delta_);
                write_varint(writable, offset_delta_);
                write_key(writable);
                write_value(writable);
                write_headers(writable);
        }

        const std::vector<unsigned char> &value() const {
                return value_;
        }

private:
        std::int32_t length_;
        std::int8_t attributes_;
        std::int64_t timestamp_delta_;
        std::int32_t offset_delta_;
        std::vector<unsigned char> value_;

        void read_key(Readable &readable) {
                if (read_varint(readable) >= 0) {
                        throw std::runtime_error("unexpected key");
                }
        }

        void write_key(Writable &writable) const {
                write_varint(writable, -1);
        }

        void read_value(Readable &readable) {
                std::int32_t value_len = read_varint(readable);
                value_.resize(value_len);
                readable.read(value_.data(), value_len);
        }

        void write_value(Writable &writable) const {
                write_varint(writable, value_.size());
                writable.write(value_.data(), value_.size());
        }

        void read_headers(Readable &readable) {
                if (read_unsigned_varint(readable) != 0) {
                        throw std::runtime_error("unexpected headers");
                }
        }

        void write_headers(Writable &writable) const {
                write_unsigned_varint(writable, 0);
        }
};

}

#endif
