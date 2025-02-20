#ifndef codecrafters_kafka_requests_fetch_response_hpp
#define codecrafters_kafka_requests_fetch_response_hpp

#include <cstdint>
#include <utility>
#include <vector>

#include "kafka/metadata/record_batch.hpp"
#include "kafka/protocol/error_code.hpp"
#include "kafka/protocol/uuid.hpp"
#include "kafka/protocol/writable.hpp"
#include "kafka/requests/response.hpp"

namespace kafka {

class AbortedTransaction {
public:
        void write(Writable &writable) const {
                write_int64(writable, producer_id_);
                write_int64(writable, first_offset_);
                write_tagged_fields(writable);
        }

private:
        std::int64_t producer_id_;
        std::int64_t first_offset_;
};

class PartitionData {
public:
        void write(Writable &writable) const {
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

        // Sets the partition index.
        void set_partition_index(std::int32_t partition_index) {
                partition_index_ = partition_index;
        }

        // Sets the error code, or 0 if there was no fetch error.
        void set_error_code(ErrorCode error_code) {
                error_code_ = error_code;
        }

        // Sets the record data.
        void set_records(std::vector<RecordBatch> records) {
                records_ = std::move(records);
        }

private:
        std::int32_t partition_index_;
        ErrorCode error_code_;
        std::int64_t high_watermark_;
        std::int64_t last_stable_offset_;
        std::int64_t log_start_offset_;
        std::vector<AbortedTransaction> aborted_transactions_;
        std::int32_t preferred_read_replica_;
        std::vector<RecordBatch> records_;
};

class FetchableTopicResponse {
public:
        void write(Writable &writable) const {
                topic_id_.write(writable);
                write_compact_array(writable, partitions_);
                write_tagged_fields(writable);
        }

        void set_topic_id(Uuid topic_id) {
                topic_id_ = std::move(topic_id);
        }

        void set_partitions(std::vector<PartitionData> partitions) {
                partitions_ = std::move(partitions);
        }

private:
        Uuid topic_id_;
        std::vector<PartitionData> partitions_;
};

// Fetch response version 16.
class FetchResponse : public Response {
public:
        // Sets the duration in milliseconds for which the request was throttled due to a quota violation, or zero if the request did not violate any quota.
        void set_throttle_time_ms(std::int32_t throttle_time_ms) {
                throttle_time_ms_ = throttle_time_ms;
        }

        // Sets the top-level response error code.
        void set_error_code(ErrorCode error_code) {
                error_code_ = error_code;
        }

        // Sets the fetch session ID, or 0 if this is not part of a fetch session.
        void set_session_id(std::int32_t session_id) {
                session_id_ = session_id;
        }

        // Sets the response topics.
        void set_responses(std::vector<FetchableTopicResponse> responses) {
                responses_ = std::move(responses);
        }

private:
        std::int32_t throttle_time_ms_;
        ErrorCode error_code_;
        std::int32_t session_id_;
        std::vector<FetchableTopicResponse> responses_;

        void write_response_body(Writable &writable) const override {
                write_int32(writable, throttle_time_ms_);
                write_error_code(writable, error_code_);
                write_int32(writable, session_id_);
                write_compact_array(writable, responses_);
                write_tagged_fields(writable);
        }
};

}

#endif
