#include "kafka/metadata/cluster_metadata.hpp"
#include "kafka/metadata/record_batch.hpp"
#include "kafka/protocol/readable.hpp"
#include "kafka/utils/bytes_io.hpp"

#include <cstdlib>
#include <err.h>
#include <stdexcept>
#include <utility>

namespace kafka {

std::string ClusterMetadata::lookup_topic_name(const Uuid &topic_id) const {
        auto it = ids_to_names_.find(topic_id);
        if (it == ids_to_names_.end()) {
                throw std::runtime_error("unknown topic ID");
        }
        return it->second;
}

Uuid ClusterMetadata::lookup_topic_id(const std::string &name) const {
        auto it = names_to_ids_.find(name);
        if (it == names_to_ids_.end()) {
                throw std::runtime_error("unknown topic name");
        }
        return it->second;
}

std::vector<std::int32_t> ClusterMetadata::lookup_partitions(const Uuid &topic_id) const {
        auto it = ids_to_partitions_.find(topic_id);
        if (it == ids_to_partitions_.end()) {
                throw std::runtime_error("unknown topic ID");
        }
        return it->second;
}

ClusterMetadata::ClusterMetadata() {
        auto record_batches = read_record_batches("__cluster_metadata", 0);
        for (const auto &record_batch : record_batches) {
                for (const auto &record : record_batch.records()) {
                        parse_record_value(record.value());
                }
        }
}

void ClusterMetadata::parse_record_value(const std::vector<unsigned char> &value) {
        BytesIO readable(value);
        std::int8_t frame_version = read_int8(readable);
        std::int8_t type = read_int8(readable);
        std::int8_t version = read_int8(readable);

        switch (type) {
        case 2: {
                std::string topic_name = read_compact_string(readable);
                Uuid topic_id;
                topic_id.read(readable);
                ids_to_names_[topic_id] = topic_name;
                names_to_ids_[topic_name] = std::move(topic_id);
                break;
        }
        case 3: {
                std::int32_t partition_id = read_int32(readable);
                Uuid topic_id;
                topic_id.read(readable);
                ids_to_partitions_[topic_id].push_back(partition_id);
                break;
        }
        case 12:
                break;
        default:
                errx(EXIT_FAILURE, "unexpected record type: %d", type);
        }
}

}
