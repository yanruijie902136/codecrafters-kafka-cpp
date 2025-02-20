#ifndef codecrafters_kafka_requests_describe_topic_partitions_response_hpp
#define codecrafters_kafka_requests_describe_topic_partitions_response_hpp

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "kafka/protocol/error_code.hpp"
#include "kafka/protocol/uuid.hpp"
#include "kafka/protocol/writable.hpp"
#include "kafka/requests/response.hpp"

namespace kafka {

class ResponsePartition {
public:
        ResponsePartition(ErrorCode error_code, std::int32_t partition_index) : error_code_(error_code), partition_index_(partition_index) {}

        void write(Writable &writable) const {
                write_error_code(writable, error_code_);
                write_int32(writable, partition_index_);
                write_int32(writable, leader_id_);
                write_int32(writable, leader_epoch_);
                auto wf = WriteObjectFunction<std::int32_t>(write_int32);
                write_compact_array(writable, replica_nodes_, wf);
                write_compact_array(writable, isr_nodes_, wf);
                write_compact_array(writable, eligible_leader_replicas_, wf);
                write_compact_array(writable, last_known_elr_, wf);
                write_compact_array(writable, offline_replicas_, wf);
                write_tagged_fields(writable);
        }

private:
        ErrorCode error_code_;
        std::int32_t partition_index_;
        std::int32_t leader_id_;
        std::int32_t leader_epoch_;
        std::vector<std::int32_t> replica_nodes_;
        std::vector<std::int32_t> isr_nodes_;
        std::vector<std::int32_t> eligible_leader_replicas_;
        std::vector<std::int32_t> last_known_elr_;
        std::vector<std::int32_t> offline_replicas_;
};

class ResponseTopic {
public:
        void write(Writable &writable) const {
                write_error_code(writable, error_code_);
                write_compact_nullable_string(writable, name_);
                topic_id_.write(writable);
                write_boolean(writable, is_internal_);
                write_compact_array(writable, partitions_);
                write_int32(writable, topic_authorized_operations_);
                write_tagged_fields(writable);
        }

        // Sets the topic error.
        void set_error_code(ErrorCode error_code) {
                error_code_ = error_code;
        }

        // Sets the topic name.
        void set_name(std::string name) {
                name_ = std::move(name);
        }

        // Sets the topic id.
        void set_topic_id(Uuid topic_id) {
                topic_id_ = std::move(topic_id);
        }

        // Sets the partitions in the topic.
        void set_partitions(std::vector<ResponsePartition> partitions) {
                partitions_ = std::move(partitions);
        }

private:
        ErrorCode error_code_;
        std::string name_;
        Uuid topic_id_;
        bool is_internal_;
        std::vector<ResponsePartition> partitions_;
        std::int32_t topic_authorized_operations_;
};

// DescribeTopicPartitions response version 0.
class DescribeTopicPartitionsResponse : public Response {
public:
        // Sets the duration in milliseconds for which the request was throttled due to a quota violation, or zero if the request did not violate any quota.
        void set_throttle_time_ms(std::int32_t throttle_time_ms) {
                throttle_time_ms_ = throttle_time_ms;
        }

        // Sets the topics in the response.
        void set_topics(std::vector<ResponseTopic> topics) {
                topics_ = std::move(topics);
        }

private:
        std::int32_t throttle_time_ms_;
        std::vector<ResponseTopic> topics_;

        void write_response_body(Writable &writable) const override {
                write_int32(writable, throttle_time_ms_);
                write_compact_array(writable, topics_);
                write_cursor(writable);
                write_tagged_fields(writable);
        }

        void write_cursor(Writable &writable) const {
                const unsigned char c = 0xFF;
                writable.write(&c, sizeof(c));
        }
};

}

#endif
