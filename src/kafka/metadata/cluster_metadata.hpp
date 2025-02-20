#ifndef codecrafters_kafka_metadata_cluster_metadata_hpp
#define codecrafters_kafka_metadata_cluster_metadata_hpp

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "kafka/protocol/uuid.hpp"

namespace kafka {

class ClusterMetadata {
public:
        // Returns the only instance of ClusterMetadata.
        static ClusterMetadata &instance() {
                static ClusterMetadata cluster_metadata;
                return cluster_metadata;
        }

        // Looks up a topic's ID. Throws runtime error on unknown topic names.
        Uuid lookup_topic_id(const std::string &name) const;

        // Looks up a topic's partitions. Throws runtime error on unknown topic IDs.
        std::vector<std::int32_t> lookup_partitions(const Uuid &topic_id) const;

private:
        std::map<std::string, Uuid> names_to_ids_;
        std::map<Uuid, std::vector<std::int32_t>> ids_to_partitions_;

        ClusterMetadata();

        ClusterMetadata(const ClusterMetadata &other) = delete;
        ClusterMetadata &operator=(const ClusterMetadata &other) = delete;
        ClusterMetadata(ClusterMetadata &&other) = delete;
        ClusterMetadata &operator=(ClusterMetadata &&other) = delete;

        void parse_record_value(const std::vector<unsigned char> &value);
};

}

#endif
