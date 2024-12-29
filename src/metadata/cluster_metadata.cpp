#include "kafka/metadata/cluster_metadata.hpp"
#include "kafka/protocol/file_descriptor.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/readable_buffer.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/utils.hpp"

#include <mutex>

namespace kafka {

std::vector<RecordBatch> read_record_batches(const std::string &topic_name, INT32 partition_index) {
    auto log_file_path = std::format(
        "/tmp/kraft-combined-logs/{}-{}/00000000000000000000.log", topic_name, partition_index);
    FileDescriptor log_fd(log_file_path.c_str(), O_RDONLY);

    std::vector<RecordBatch> record_batches;
    for ( ; ; ) {
        RecordBatch record_batch;
        try {
            record_batch.read(log_fd);
        } catch (...) {
            return record_batches;
        }
        record_batches.push_back(std::move(record_batch));
    }
}

UUID ClusterMetadata::get_topic_id(const std::string &topic_name) const {
    std::lock_guard<std::mutex> guard(mutex_);
    auto iter = topic_ids_.find(topic_name);
    if (iter == topic_ids_.end()) {
        throw_runtime_error("unknown topic name");
    }
    return iter->second;
}

std::string ClusterMetadata::get_topic_name(const UUID &topic_id) const {
    std::lock_guard<std::mutex> guard(mutex_);
    auto iter = topic_names_.find(topic_id);
    if (iter == topic_names_.end()) {
        throw_runtime_error("unknown topic id");
    }
    return iter->second;
}

std::vector<INT32> ClusterMetadata::get_partition_ids(const UUID &topic_id) const {
    std::lock_guard<std::mutex> guard(mutex_);
    auto iter = partition_ids_.find(topic_id);
    if (iter == partition_ids_.end()) {
        throw_runtime_error("unknown topic id");
    }
    return iter->second;
}

ClusterMetadata::ClusterMetadata() {
    for (const auto &record_batch : read_record_batches("__cluster_metadata", 0)) {
        for (const auto &record : record_batch.records()) {
            ReadableBuffer rb(record.value());
            INT8 frame_version = read_int8(rb);
            INT8 type = read_int8(rb);
            INT8 version = read_int8(rb);

            if (type == 2) {
                COMPACT_STRING topic_name = read_compact_string(rb);
                UUID topic_id = read_uuid(rb);
                topic_ids_[topic_name] = topic_id;
                topic_names_[topic_id] = topic_name;
            } else if (type == 3) {
                INT32 partition_id = read_int32(rb);
                UUID topic_id = read_uuid(rb);
                partition_ids_[topic_id].push_back(partition_id);
            }
        }
    }
}

}
