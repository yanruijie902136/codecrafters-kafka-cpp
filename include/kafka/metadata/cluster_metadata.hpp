#ifndef CODECRAFTERS_KAFKA_METADATA_CLUSTER_METADATA_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_METADATA_CLUSTER_METADATA_HPP_INCLUDED

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>

#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/readable_buffer.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/protocol/uuid.hpp"

namespace kafka {

class RecordHeader {
public:
    // Reads this `RecordHeader` from a byte stream.
    void read(IReadable &readable) {
        throw std::runtime_error("unexpected record header");
    }
};

class Record {
public:
    // Reads this `Record` from a byte stream.
    void read(IReadable &readable) {
        length_ = read_varint(readable);
        if (length_ < 0) {
            return;
        }

        BYTES record_bytes(length_);
        readable.read(record_bytes.data(), record_bytes.size());
        ReadableBuffer rb(std::move(record_bytes));
        attributes_ = read_int8(rb);
        timestamp_delta_ = read_varlong(rb);
        offset_delta_ = read_varint(rb);

        key_length_ = read_varint(rb);
        key_.resize(key_length_ + 1);
        rb.read(key_.data(), key_.size());

        value_len_ = read_varint(rb);
        if (value_len_ < 0) {
            value_.clear();
        } else {
            value_.resize(value_len_);
            rb.read(value_.data(), value_.size());
        }

        headers_ = read_compact_array<RecordHeader>(rb);
    }

    const BYTES &value() const {
        return value_;
    }

private:
    VARINT length_;
    INT8 attributes_;
    VARLONG timestamp_delta_;
    VARINT offset_delta_;
    VARINT key_length_;
    BYTES key_;
    VARINT value_len_;
    BYTES value_;
    COMPACT_ARRAY<RecordHeader> headers_;
};

class RecordBatch {
public:
    // Reads this `RecordBatch` from a byte stream.
    void read(IReadable &readable) {
        base_offset_ = read_int64(readable);
        ReadableBuffer rb(read_bytes(readable));
        partition_leader_epoch_ = read_int32(rb);
        magic_ = read_int8(rb);
        crc_ = read_uint32(rb);
        attributes_ = read_int16(rb);
        last_offset_delta_ = read_int32(rb);
        base_timestamp_ = read_int64(rb);
        max_timestamp_ = read_int64(rb);
        producer_id_ = read_int64(rb);
        producer_epoch_ = read_int16(rb);
        base_sequence_ = read_int32(rb);
        records_ = read_array<Record>(rb);
    }

    const ARRAY<Record> &records() const {
        return records_;
    }

private:
    INT64 base_offset_;
    // INT32 batch_length_;
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

class ClusterMetadata {
public:
    // Returns the only instance of `ClusterMetadata`.
    static const ClusterMetadata &get_instance() {
        static ClusterMetadata cluster_metadata;
        return cluster_metadata;
    }

    // Gets the UUID of the topic with the specified name.
    UUID get_topic_id(const std::string &topic_name) const;

    // Gets the partition IDs of the topic with the specified UUID.
    std::vector<INT32> get_partition_ids(const UUID &topic_id) const;

private:
    mutable std::mutex mutex_;
    std::map<std::string, UUID> topic_ids_;
    std::map<UUID, std::vector<INT32>, UUIDCompare> partition_ids_;

    ClusterMetadata();

    ClusterMetadata(const ClusterMetadata &other) = delete;
    ClusterMetadata &operator=(const ClusterMetadata &other) = delete;
    ClusterMetadata(ClusterMetadata &&other) = delete;
    ClusterMetadata &operator=(ClusterMetadata &&other) = delete;
};

}

#endif  // CODECRAFTERS_KAFKA_METADATA_CLUSTER_METADATA_HPP_INCLUDED