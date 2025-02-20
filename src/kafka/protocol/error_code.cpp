#include "kafka/protocol/error_code.hpp"
#include "kafka/protocol/writable.hpp"

#include <utility>

namespace kafka {

void write_error_code(Writable &writable, ErrorCode error_code) {
        write_int16(writable, std::to_underlying(error_code));
}

}
