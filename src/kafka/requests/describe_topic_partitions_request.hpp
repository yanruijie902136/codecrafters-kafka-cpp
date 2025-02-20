#ifndef codecrafters_kafka_requests_describe_topic_partitions_request_hpp
#define codecrafters_kafka_requests_describe_topic_partitions_request_hpp

#include <stdexcept>
#include <string>
#include <vector>

#include "kafka/protocol/readable.hpp"
#include "kafka/requests/request.hpp"

namespace kafka {

class TopicRequest {
public:
        void read(Readable &readable) {
                name_ = read_compact_string(readable);
                read_tagged_fields(readable);
        }

        // The topic name.
        const std::string &name() const {
                return name_;
        }

private:
        std::string name_;
};

// DescribeTopicPartitions request version 0.
class DescribeTopicPartitionsRequest : public Request {
public:
        // The topics to fetch details for.
        const std::vector<TopicRequest> &topics() const {
                return topics_;
        }

private:
        std::vector<TopicRequest> topics_;
        std::int32_t response_partition_limit_;

        void read_request_body(Readable &readable) override {
                topics_ = read_compact_array<TopicRequest>(readable);
                response_partition_limit_ = read_int32(readable);
                read_cursor(readable);
                read_tagged_fields(readable);
        }

        void read_cursor(Readable &readable) {
                unsigned char c;
                readable.read(&c, sizeof(c));
                if (c != 0xFF) {
                        throw std::runtime_error("unexpected cursor");
                }
        }
};

}

#endif
