#ifndef codecrafters_kafka_requests_fetch_request_hpp
#define codecrafters_kafka_requests_fetch_request_hpp

#include <cstdint>
#include <string>
#include <vector>

#include "kafka/protocol/readable.hpp"
#include "kafka/protocol/uuid.hpp"
#include "kafka/requests/request.hpp"

namespace kafka {

class FetchPartition {
public:
        void read(Readable &readable) {
                partition_ = read_int32(readable);
                current_leader_epoch_ = read_int32(readable);
                fetch_offset_ = read_int64(readable);
                last_fetched_epoch_ = read_int32(readable);
                log_start_offset_ = read_int64(readable);
                partition_max_bytes_ = read_int32(readable);
                read_tagged_fields(readable);
        }

        // The partition index.
        std::int32_t partition() const {
                return partition_;
        }

private:
        std::int32_t partition_;
        std::int32_t current_leader_epoch_;
        std::int64_t fetch_offset_;
        std::int32_t last_fetched_epoch_;
        std::int64_t log_start_offset_;
        std::int32_t partition_max_bytes_;
};

class FetchTopic {
public:
        void read(Readable &readable) {
                topic_id_.read(readable);
                partitions_ = read_compact_array<FetchPartition>(readable);
                read_tagged_fields(readable);
        }

        // The unique topic ID.
        const Uuid &topic_id() const {
                return topic_id_;
        }

        // The partitions to fetch.
        const std::vector<FetchPartition> &partitions() const {
                return partitions_;
        }

private:
        Uuid topic_id_;
        std::vector<FetchPartition> partitions_;
};

class ForgottenTopic {
public:
        void read(Readable &readable) {
                topic_id_.read(readable);
                partitions_ = read_compact_array(readable, ReadObjectFunction<std::int32_t>(read_int32));
                read_tagged_fields(readable);
        }

private:
        Uuid topic_id_;
        std::vector<std::int32_t> partitions_;
};

// Fetch request version 16.
class FetchRequest : public Request {
public:
        // The topics to fetch.
        const std::vector<FetchTopic> &topics() const {
                return topics_;
        }

private:
        std::int32_t max_wait_ms_;
        std::int32_t min_bytes_;
        std::int32_t max_bytes_;
        std::int8_t isolation_level_;
        std::int32_t session_id_;
        std::int32_t session_epoch_;
        std::vector<FetchTopic> topics_;
        std::vector<ForgottenTopic> forgotten_topics_data_;
        std::string rack_id_;

        void read_request_body(Readable &readable) override {
                max_wait_ms_ = read_int32(readable);
                min_bytes_ = read_int32(readable);
                max_bytes_ = read_int32(readable);
                isolation_level_ = read_int8(readable);
                session_id_ = read_int32(readable);
                session_epoch_ = read_int32(readable);
                topics_ = read_compact_array<FetchTopic>(readable);
                forgotten_topics_data_ = read_compact_array<ForgottenTopic>(readable);
                rack_id_ = read_compact_string(readable);
                read_tagged_fields(readable);
        }
};

}

#endif
