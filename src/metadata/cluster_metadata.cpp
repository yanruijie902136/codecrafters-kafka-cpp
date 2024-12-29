#include "kafka/metadata/cluster_metadata.hpp"
#include "kafka/protocol/file_descriptor.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/readable_buffer.hpp"
#include "kafka/protocol/types.hpp"
#include "kafka/utils.hpp"

#include <mutex>

namespace kafka {

UUID ClusterMetadata::get_topic_id(const std::string &topic_name) const {
    std::lock_guard<std::mutex> guard(mutex_);
    auto iter = topic_ids_.find(topic_name);
    if (iter == topic_ids_.end()) {
        throw_runtime_error("unknown topic name");
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
    FileDescriptor log_fd("/tmp/kraft-combined-logs/__cluster_metadata-0/00000000000000000000.log", O_RDONLY);

    for ( ; ; ) {
        RecordBatch record_batch;
        try {
            record_batch.read(log_fd);
        } catch (...) {
            return;
        }

        for (const auto &record : record_batch.records()) {
            if (record.value().empty()) {
                continue;
            }
            ReadableBuffer rb(record.value());
            INT8 frame_version = read_int8(rb);
            INT8 type = read_int8(rb);
            INT8 version = read_int8(rb);

            if (type == 2) {
                COMPACT_STRING topic_name = read_compact_string(rb);
                UUID topic_id = read_uuid(rb);
                topic_ids_[topic_name] = topic_id;
                partition_ids_[topic_id] = {};
            } else if (type == 3) {
                INT32 partition_id = read_int32(rb);
                UUID topic_id = read_uuid(rb);
                partition_ids_[topic_id].push_back(partition_id);
            } else if (type == 12) {
                COMPACT_STRING feature_name = read_compact_string(rb);
                INT16 feature_level = read_int16(rb);
            }
        }
    }
}

}
