#ifndef codecrafters_kafka_metadata_record_batch_hpp
#define codecrafters_kafka_metadata_record_batch_hpp

#include <cstdint>
#include <vector>

#include "kafka/metadata/record.hpp"
#include "kafka/protocol/readable.hpp"
#include "kafka/protocol/writable.hpp"

namespace kafka {

class RecordBatch {
public:
        void read(Readable &readable) {
                base_offset_ = read_int64(readable);
                batch_length_ = read_int32(readable);
                partition_leader_epoch_ = read_int32(readable);
                magic_ = read_int8(readable);
                crc_ = read_uint32(readable);
                attributes_ = read_int16(readable);
                last_offset_delta_ = read_int32(readable);
                base_timestamp_ = read_int64(readable);
                max_timestamp_ = read_int64(readable);
                producer_id_ = read_int64(readable);
                producer_epoch_ = read_int16(readable);
                base_sequence_ = read_int32(readable);
                records_ = read_array<Record>(readable);
        }

        void write(Writable &writable) const {
                write_int64(writable, base_offset_);
                write_int32(writable, batch_length_);
                write_int32(writable, partition_leader_epoch_);
                write_int8(writable, magic_);
                write_uint32(writable, crc_);
                write_int16(writable, attributes_);
                write_int32(writable, last_offset_delta_);
                write_int64(writable, base_timestamp_);
                write_int64(writable, max_timestamp_);
                write_int64(writable, producer_id_);
                write_int16(writable, producer_epoch_);
                write_int32(writable, base_sequence_);
                write_array(writable, records_);
        }

        const std::vector<Record> &records() const {
                return records_;
        }

private:
        std::int64_t base_offset_;
        std::int32_t batch_length_;
        std::int32_t partition_leader_epoch_;
        std::int8_t magic_;
        std::uint32_t crc_;
        std::int16_t attributes_;
        std::int32_t last_offset_delta_;
        std::int64_t base_timestamp_;
        std::int64_t max_timestamp_;
        std::int64_t producer_id_;
        std::int16_t producer_epoch_;
        std::int32_t base_sequence_;
        std::vector<Record> records_;
};

}

#endif
