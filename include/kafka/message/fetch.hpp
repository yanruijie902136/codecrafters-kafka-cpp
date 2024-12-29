#ifndef CODECRAFTERS_KAFKA_MESSAGE_FETCH_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_MESSAGE_FETCH_HPP_INCLUDED

#include "kafka/message/abstract.hpp"
#include "kafka/metadata/cluster_metadata.hpp"
#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/iwritable.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/protocol/uuid.hpp"

namespace kafka {

class FetchRequest : public AbstractRequest {
public:
    class FetchPartition {
    public:
        // Reads this `FetchPartition` from a byte stream.
        void read(IReadable &readable) {
            partition_ = read_int32(readable);
            current_leader_epoch_ = read_int32(readable);
            fetch_offset_ = read_int64(readable);
            last_fetched_epoch_ = read_int32(readable);
            log_start_offset_ = read_int64(readable);
            partition_max_bytes_ = read_int32(readable);
            read_tagged_fields(readable);
        }

        const INT32 &partition() const {
            return partition_;
        }

    private:
        INT32 partition_;
        INT32 current_leader_epoch_;
        INT64 fetch_offset_;
        INT32 last_fetched_epoch_;
        INT64 log_start_offset_;
        INT32 partition_max_bytes_;
    };

    class FetchTopic {
    public:
        // Reads this `FetchTopic` from a byte stream.
        void read(IReadable &readable) {
            topic_id_ = read_uuid(readable);
            partitions_ = read_compact_array<FetchPartition>(readable);
            read_tagged_fields(readable);
        }

        const UUID &topic_id() const {
            return topic_id_;
        }

        const COMPACT_ARRAY<FetchPartition> &partitions() const {
            return partitions_;
        }

    private:
        UUID topic_id_;
        COMPACT_ARRAY<FetchPartition> partitions_;
    };

    class ForgottonTopic {
    public:
        // Reads this `ForgottonTopic` from a byte stream.
        void read(IReadable &readable) {
            topic_id_ = read_uuid(readable);
            partitions_ = read_compact_array(readable, ReadObjectFunction<INT32>(read_int32));
            read_tagged_fields(readable);
        }

    private:
        UUID topic_id_;
        COMPACT_ARRAY<INT32> partitions_;
    };

    // Reads this `FetchRequest` from a byte stream.
    void read(IReadable &readable) override {
        max_wait_ms_ = read_int32(readable);
        min_bytes_ = read_int32(readable);
        max_bytes_ = read_int32(readable);
        isolation_level_ = read_int8(readable);
        session_id_ = read_int32(readable);
        session_epoch_ = read_int32(readable);
        topics_ = read_compact_array<FetchTopic>(readable);
        forgotten_topics_data_ = read_compact_array<ForgottonTopic>(readable);
        rack_id_ = read_compact_string(readable);
        read_tagged_fields(readable);
    }

    const COMPACT_ARRAY<FetchTopic> &topics() const {
        return topics_;
    }

private:
    INT32 max_wait_ms_;
    INT32 min_bytes_;
    INT32 max_bytes_;
    INT8 isolation_level_;
    INT32 session_id_;
    INT32 session_epoch_;
    COMPACT_ARRAY<FetchTopic> topics_;
    COMPACT_ARRAY<ForgottonTopic> forgotten_topics_data_;
    COMPACT_STRING rack_id_;
};

class FetchResponse : public AbstractResponse {
public:
    class AbortedTransaction {
    public:
        // Writes this `AbortedTransaction` to a byte stream.
        void write(IWritable &writable) const {
            write_int64(writable, producer_id_);
            write_int64(writable, first_offset_);
            write_tagged_fields(writable);
        }

    private:
        INT64 producer_id_;
        INT64 first_offset_;
    };

    class PartitionData {
    public:
        // Writes this `PartitionData` to a byte stream.
        void write(IWritable &writable) const {
            write_int32(writable, partition_index_);
            write_error_code(writable, error_code_);
            write_int64(writable, high_watermark_);
            write_int64(writable, last_stable_offset_);
            write_int64(writable, log_start_offset_);
            write_compact_array(writable, aborted_transactions_);
            write_int32(writable, preferred_read_replica_);
            write_compact_array(writable, records_);
            write_tagged_fields(writable);
        }

        INT32 &partition_index() {
            return partition_index_;
        }

        ErrorCode &error_code() {
            return error_code_;
        }

        COMPACT_ARRAY<RecordBatch> &records() {
            return records_;
        }

    private:
        INT32 partition_index_;
        ErrorCode error_code_;
        INT64 high_watermark_;
        INT64 last_stable_offset_;
        INT64 log_start_offset_;
        COMPACT_ARRAY<AbortedTransaction> aborted_transactions_;
        INT32 preferred_read_replica_;
        COMPACT_ARRAY<RecordBatch> records_;
    };

    class FetchableTopicResponse {
    public:
        // Writes this `FetchableTopicResponse` to a byte stream.
        void write(IWritable &writable) const {
            write_uuid(writable, topic_id_);
            write_compact_array(writable, partitions_);
            write_tagged_fields(writable);
        }

        UUID &topic_id() {
            return topic_id_;
        }

        COMPACT_ARRAY<PartitionData> &partitions() {
            return partitions_;
        }

    private:
        UUID topic_id_;
        COMPACT_ARRAY<PartitionData> partitions_;
    };

    // The API key of this `FetchResponse`.
    constexpr ApiKey api_key() const override {
        return ApiKey::FETCH;
    }

    // Writes this `FetchResponse` to a byte stream.
    void write(IWritable &writable) const override {
        write_int32(writable, throttle_time_ms_);
        write_error_code(writable, error_code_);
        write_int32(writable, session_id_);
        write_compact_array(writable, responses_);
        write_tagged_fields(writable);
    }

    INT32 &throttle_time_ms() {
        return throttle_time_ms_;
    }

    ErrorCode &error_code() {
        return error_code_;
    }

    INT32 &session_id() {
        return session_id_;
    }

    COMPACT_ARRAY<FetchableTopicResponse> &responses() {
        return responses_;
    }

private:
    INT32 throttle_time_ms_;
    ErrorCode error_code_;
    INT32 session_id_;
    COMPACT_ARRAY<FetchableTopicResponse> responses_;
};

}

#endif  // CODECRAFTERS_KAFKA_MESSAGE_FETCH_HPP_INCLUDED
