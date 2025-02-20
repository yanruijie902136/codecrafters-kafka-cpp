#include "kafka/protocol/api_key.hpp"
#include "kafka/protocol/readable.hpp"
#include "kafka/protocol/writable.hpp"

#include <utility>

namespace kafka {

ApiKey read_api_key(Readable &readable) {
        return static_cast<ApiKey>(read_int16(readable));
}

void write_api_key(Writable &writable, ApiKey api_key) {
        write_int16(writable, std::to_underlying(api_key));
}

}
