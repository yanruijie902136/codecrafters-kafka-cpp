#include "kafka/metadata/record_batch.hpp"
#include "kafka/utils/bytes_io.hpp"

#include <format>
#include <vector>

namespace kafka {

std::vector<RecordBatch> read_record_batches(const std::string &topic_name, std::int32_t partition_id) {
        auto path = std::format(
                "/tmp/kraft-combined-logs/{}-{}/00000000000000000000.log", topic_name, partition_id
        );
        BytesIO readable(path);
        std::vector<RecordBatch> record_batches;
        while (readable.remaining() > 0) {
                record_batches.emplace_back();
                record_batches.back().read(readable);
        }
        return record_batches;
}

}
