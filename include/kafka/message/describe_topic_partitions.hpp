#ifndef CODECRAFTERS_KAFKA_MESSAGE_DESCRIBE_TOPIC_PARTITIONS_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_MESSAGE_DESCRIBE_TOPIC_PARTITIONS_HPP_INCLUDED

#include <stdexcept>

#include "kafka/message/abstract.hpp"
#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/iwritable.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/protocol/uuid.hpp"

namespace kafka {

class DescribeTopicPartitionsRequest : public AbstractRequest {
public:
    class TopicRequest {
    public:
        // Reads this `TopicRequest` from a byte stream.
        void read(IReadable &readable) {
            name_ = read_compact_string(readable);
            read_tagged_fields(readable);
        }

        const COMPACT_STRING &name() const {
            return name_;
        }

    private:
        COMPACT_STRING name_;
    };

    // Reads this `DescribeTopicPartitionsRequest` from a byte stream.
    void read(IReadable &readable) override {
        topics_ = read_compact_array<TopicRequest>(readable);
        response_partition_limit_ = read_int32(readable);
        unsigned char c;
        readable.read(&c, sizeof(c));
        if (c != 0xFF) {
            throw std::runtime_error("unexpected cursor");
        }
        read_tagged_fields(readable);
    }

    const COMPACT_ARRAY<TopicRequest> &topics() const {
        return topics_;
    }

    const INT32 &response_partition_limit() const {
        return response_partition_limit_;
    }

private:
    COMPACT_ARRAY<TopicRequest> topics_;
    INT32 response_partition_limit_;
};

class DescribeTopicPartitionsResponse : public AbstractResponse {
public:
    class ResponsePartition {
    public:
        ResponsePartition(ErrorCode error_code, INT32 partition_index)
            : error_code_(error_code), partition_index_(partition_index) {}

        // Writes this `ResponsePartition` to a byte stream.
        void write(IWritable &writable) const {
            write_error_code(writable, error_code_);
            write_int32(writable, partition_index_);
            write_int32(writable, leader_id_);
            write_int32(writable, leader_epoch_);
            auto write_int32_function = WriteObjectFunction<INT32>(write_int32);
            write_compact_array(writable, replica_nodes_, write_int32_function);
            write_compact_array(writable, isr_nodes_, write_int32_function);
            write_compact_array(writable, eligible_leader_replicas_, write_int32_function);
            write_compact_array(writable, last_known_elr_, write_int32_function);
            write_compact_array(writable, offline_replicas_, write_int32_function);
            write_tagged_fields(writable);
        }

    private:
        ErrorCode error_code_;
        INT32 partition_index_;
        INT32 leader_id_;
        INT32 leader_epoch_;
        COMPACT_ARRAY<INT32> replica_nodes_;
        COMPACT_ARRAY<INT32> isr_nodes_;
        COMPACT_ARRAY<INT32> eligible_leader_replicas_;
        COMPACT_ARRAY<INT32> last_known_elr_;
        COMPACT_ARRAY<INT32> offline_replicas_;
    };

    class ResponseTopic {
    public:
        // Writes this `ResponseTopic` to a byte stream.
        void write(IWritable &writable) const {
            write_error_code(writable, error_code_);
            write_compact_nullable_string(writable, name_);
            write_uuid(writable, topic_id_);
            write_boolean(writable, is_internal_);
            write_compact_array(writable, partitions_);
            write_int32(writable, topic_authorized_operations_);
            write_tagged_fields(writable);
        }

        ErrorCode &error_code() {
            return error_code_;
        }

        COMPACT_NULLABLE_STRING &name() {
            return name_;
        }

        UUID &topic_id() {
            return topic_id_;
        }

        COMPACT_ARRAY<ResponsePartition> &partitions() {
            return partitions_;
        }

    private:
        ErrorCode error_code_;
        COMPACT_NULLABLE_STRING name_;
        UUID topic_id_;
        BOOLEAN is_internal_;
        COMPACT_ARRAY<ResponsePartition> partitions_;
        INT32 topic_authorized_operations_;
    };

    // The API key of this `DescribeTopicPartitionsResponse`.
    constexpr ApiKey api_key() const override {
        return ApiKey::DESCRIBE_TOPIC_PARTITIONS;
    }

    // Writes this `DescribeTopicPartitionsResponse` to a byte stream.
    void write(IWritable &writable) const override {
        write_int32(writable, throttle_time_ms_);
        write_compact_array(writable, topics_);
        static constexpr unsigned char c = 0xFF;
        writable.write(&c, sizeof(c));
        write_tagged_fields(writable);
    }

    INT32 &throttle_time_ms() {
        return throttle_time_ms_;
    }

    COMPACT_ARRAY<ResponseTopic> &topics() {
        return topics_;
    }

private:
    INT32 throttle_time_ms_;
    COMPACT_ARRAY<ResponseTopic> topics_;
};

}

#endif  // CODECRAFTERS_KAFKA_MESSAGE_DESCRIBE_TOPIC_PARTITIONS_HPP_INCLUDED
