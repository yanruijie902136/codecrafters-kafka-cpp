#ifndef CODECRAFTERS_KAFKA_METADATA_CLUSTER_METADATA_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_METADATA_CLUSTER_METADATA_HPP_INCLUDED

#include <map>
#include <mutex>
#include <string>

#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/iwritable.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/protocol/uuid.hpp"
#include "kafka/utils.hpp"

namespace kafka {

class Record {
public:
    // Reads this `Record` from a byte stream.
    void read(IReadable &readable) {
        length_ = read_varint(readable);
        attributes_ = read_int8(readable);
        timestamp_delta_ = read_varlong(readable);
        offset_delta_ = read_varint(readable);

        unsigned char c;
        readable.read(&c, sizeof(c));
        if (c != 0x01) {
            throw_runtime_error("unexpected key");
        }

        value_len_ = read_varint(readable);
        value_.resize(value_len_);
        readable.read(value_.data(), value_.size());

        readable.read(&c, sizeof(c));
        if (c != 0x00) {
            throw_runtime_error("unexpected record header");
        }
    }

    // Writes this `Record` to a byte stream.
    void write(IWritable &writable) const {
        write_varint(writable, length_);
        write_int8(writable, attributes_);
        write_varlong(writable, timestamp_delta_);
        write_varint(writable, offset_delta_);

        unsigned char c = 0x01;
        writable.write(&c, sizeof(c));

        write_varint(writable, value_len_);
        writable.write(value_.data(), value_.size());

        c = 0x00;
        writable.write(&c, sizeof(c));
    }

    const BYTES &value() const {
        return value_;
    }

private:
    VARINT length_;
    INT8 attributes_;
    VARLONG timestamp_delta_;
    VARINT offset_delta_;
    VARINT value_len_;
    BYTES value_;
};

class RecordBatch {
public:
    // Reads this `RecordBatch` from a byte stream.
    void read(IReadable &readable) {
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

    // Writes this `RecordBatch` to a byte stream.
    void write(IWritable &writable) const {
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

    const ARRAY<Record> &records() const {
        return records_;
    }

private:
    INT64 base_offset_;
    INT32 batch_length_;
    INT32 partition_leader_epoch_;
    INT8 magic_;
    UINT32 crc_;
    INT16 attributes_;
    INT32 last_offset_delta_;
    INT64 base_timestamp_;
    INT64 max_timestamp_;
    INT64 producer_id_;
    INT16 producer_epoch_;
    INT32 base_sequence_;
    ARRAY<Record> records_;
};

// Reads every `RecordBatch` that belongs to a given partition.
std::vector<RecordBatch> read_record_batches(const std::string &topic_name, INT32 partition_index);

class ClusterMetadata {
public:
    // Returns the only instance of `ClusterMetadata`.
    static const ClusterMetadata &get_instance() {
        static ClusterMetadata cluster_metadata;
        return cluster_metadata;
    }

    // Gets the UUID of the topic with the specified name.
    UUID get_topic_id(const std::string &topic_name) const;

    // Gets the name of the topic with the specified UUID.
    std::string get_topic_name(const UUID &topic_id) const;

    // Gets the partition IDs of the topic with the specified UUID.
    std::vector<INT32> get_partition_ids(const UUID &topic_id) const;

private:
    mutable std::mutex mutex_;
    std::map<std::string, UUID> topic_ids_;
    std::map<UUID, std::string, UUIDCompare> topic_names_;
    std::map<UUID, std::vector<INT32>, UUIDCompare> partition_ids_;

    ClusterMetadata();

    ClusterMetadata(const ClusterMetadata &other) = delete;
    ClusterMetadata &operator=(const ClusterMetadata &other) = delete;
    ClusterMetadata(ClusterMetadata &&other) = delete;
    ClusterMetadata &operator=(ClusterMetadata &&other) = delete;
};

}

#endif  // CODECRAFTERS_KAFKA_METADATA_CLUSTER_METADATA_HPP_INCLUDED
