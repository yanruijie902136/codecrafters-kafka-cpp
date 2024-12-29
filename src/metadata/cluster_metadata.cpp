#include "kafka/metadata/cluster_metadata.hpp"
#include "kafka/protocol/file_descriptor.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/readable_buffer.hpp"
#include "kafka/protocol/types.hpp"

#include <mutex>
#include <stdexcept>

namespace kafka {

UUID ClusterMetadata::get_topic_id(const std::string &topic_name) const {
    std::lock_guard<std::mutex> guard(mutex_);
    auto iter = topic_ids_.find(topic_name);
    if (iter == topic_ids_.end()) {
        throw std::runtime_error("unknown topic name");
    }
    return iter->second;
}

std::vector<INT32> ClusterMetadata::get_partition_ids(const UUID &topic_id) const {
    std::lock_guard<std::mutex> guard(mutex_);
    auto iter = partition_ids_.find(topic_id);
    if (iter == partition_ids_.end()) {
        throw std::runtime_error("unknown topic id");
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

/*

base offset = 00 00 00 00 00 00 00 01
batch length = 00 00 00 4F
partition leader epoch = 00 00 00 01
magic = 02
crc = B0 69 45 7C
attributes = 00 00
last offset delta = 00 00 00 00
base timestamp = 00 00 01 91 E0 5A F8 18
max timestamp = 00 00 01 91 E0 5A F8 18
producer id = FF FF FF FF FF FF FF FF
producer epoch = FF FF
base sequence = FF FF FF FF
records length = 00 00 00 01
    length = 3A
    attributes = 00
    timestamp delta = 00
    offset delta = 00
    key length = 01
    key = null
    value len = 2e
        frame version = 01
        type = 0C
        version = 00
        feature name length = 11
        feature name = 6D 65 74 61 64 61 74 61 2E 76 65 72 73 69 6F 6E
        feature level = 00 14
        tagged fields count = 00
    headers array count = 00

base offset = 00 00 00 00 00 00 00 02
batch length = 00 00 00 9A
partition leader epoch = 00 00 00 01
magic = 02
crc = 96 4A CD 2F
attributes = 00 00
last offset delta = 00 00 00 01
base timestamp = 00 00 01 91 E0 5B 2D 15
max timestamp = 00 00 01 91 E0 5B 2D 15
producer id = FF FF FF FF FF FF FF FF
producer epoch = FF FF
base sequence = FF FF FF FF
records length = 00 00 00 02
    length = 3C
    attributes = 00
    timestamp delta = 00
    offset delta = 00
    key length = 01
    key = null
    value len = 30
        frame version = 01
        type = 02
        version = 00
        topic name length = 04
        topic name = 62 61 7A
        topic UUID = 00 00 00 00 00 00 40 00 80 00 00 00 00 00 00 86
        tagged fields count = 00
    headers array count = 00

    length = 90 01  (BUG IS HERE !!!)
    attributes = 00
    timestamp delta = 00
    offset delta = 02
    key length = 01
    key = null
    value len = 82 01
        frame version = 01
        type = 03
        version = 01
        partition id = 00 00 00 00
        topic UUID = 00 00 00 00 00 00 40 00 80 00 00 00 00 00 00 86
        length of replica array = 02
        replica array = 00 00 00 01
        length of in sync replica array = 02
        in sync replica array = 00 00 00 01
        length of removing replicas array = 01
        length of adding replicas array = 01
        leader id = 00 00 00 01
        leader epoch = 00 00 00 00
        partition epoch = 00 00 00 00
        length of directories array = 02
        directories array = 10 00 00 00 00 00 40 00 80 00 00 00 00 00 00 01
        tagged fields count = 00
    headers array count = 00

base offset = 00 00 00 00 00 00 00 04
batch length = 00 00 00 9A
partition leader epoch = 00 00 00 01
magic = 02
crc = E0 AD 41 9C
attributes = 00 00
last offset delta = 00 00 00 01
base timestamp = 00 00 01 91 E0 5B 2D 15
max timestamp = 00 00 01 91 E0 5B 2D 15
producer id = FF FF FF FF FF FF FF FF
producer epoch = FF FF
base sequence = FF FF FF FF
records length = 00 00 00 02
    length = 3C
    attributes = 00
    timestamp delta = 00
    offset delta = 00
    key length = 01
    key = null
    value len = 30
        frame version = 01
        type = 02
        version = 00
        topic name length = 04
        topic name = 66 6F 6F
        topic UUID = 00 00 00 00 00 00 40 00 80 00 00 00 00 00 00 81
        tagged fields count = 00
    headers array count = 00

    length = 90 01  (BUG IS HERE !!!)
    attributes = 00
    timestamp delta = 00
    offset delta = 02
    key length = 01
    key = null
    value len = 82 01
        frame version = 01
        type = 03
        version = 01
        partition id = 00 00 00 00
        topic UUID = 00 00 00 00 00 00 40 00 80 00 00 00 00 00 00 81
        length of replica array = 02
        replica array = 00 00 00 01
        length of in sync replica array = 02
        in sync replica array = 00 00 00 01
        length of removing replicas array = 01
        length of adding replicas array = 01
        leader id = 00 00 00 01
        leader epoch = 00 00 00 00
        partition epoch = 00 00 00 00
        length of directories array = 02
        directories array = 10 00 00 00 00 00 40 00 80 00 00 00 00 00 00 01
        tagged fields count = 00
    headers array count = 00

base offset = 00 00 00 00 00 00 00 06
batch length = 00 00 00 E4
partition leader epoch = 00 00 00 01
magic = 02
crc = F0 DB EA 05
attributes = 00 00
last offset delta = 00 00 00 02
base timestamp = 00 00 01 91 E0 5B 2D 15
max timestamp = 00 00 01 91 E0 5B 2D 15
producer id = FF FF FF FF FF FF FF FF
producer epoch = FF FF
base sequence = FF FF FF FF
records length = 00 00 00 03
    length = 3C
    attributes = 00
    timestamp delta = 00
    offset delta = 00
    key length = 01
    key = null
    value len = 30
        frame version = 01
        type = 02
        version = 00
        topic name length = 04
        topic name = 71 75 78
        topic UUID = 00 00 00 00 00 00 40 00 80 00 00 00 00 00 00 48
        tagged fields count = 00
    headers array count = 00

    length = 90 01  (BUG IS HERE !!!)
    attributes = 00
    timestamp delta = 00
    offset delta = 02
    key length = 01
    key = null
    value len = 82 01
        frame version = 01
        type = 03
        version = 01
        partition id = 00 00 00 00
        topic UUID = 00 00 00 00 00 00 40 00 80 00 00 00 00 00 00 48
        length of replica array = 02
        replica array = 00 00 00 01
        length of in sync replica array = 02
        in sync replica array = 00 00 00 01
        length of removing replicas array = 01
        length of adding replicas array = 01
        leader id = 00 00 00 01
        leader epoch = 00 00 00 00
        partition epoch = 00 00 00 00
        length of directories array = 02
        directories array = 10 00 00 00 00 00 40 00 80 00 00 00 00 00 00 01
        tagged fields count = 00
    headers array count = 00

    length = 90 01  (BUG IS HERE !!!)
    attributes = 00
    timestamp delta = 00
    offset delta = 04
    key length = 01
    key = null
    value len = 82 01
        frame version = 01
        type = 03
        version = 01
        partition id = 00 00 00 01
        topic UUID = 00 00 00 00 00 00 40 00 80 00 00 00 00 00 00 48
        length of replica array = 02
        replica array = 00 00 00 01
        length of in sync replica array = 02
        in sync replica array = 00 00 00 01
        length of removing replicas array = 01
        length of adding replicas array = 01
        leader id = 00 00 00 01
        leader epoch = 00 00 00 00
        partition epoch = 00 00 00 00
        length of directories array = 02
        directories array = 10 00 00 00 00 00 40 00 80 00 00 00 00 00 00 01
        tagged fields count = 00
    headers array count = 00

 */
