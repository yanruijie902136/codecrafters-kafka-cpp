#ifndef codecrafters_kafka_requests_response_hpp
#define codecrafters_kafka_requests_response_hpp

#include <utility>

#include "kafka/protocol/writable.hpp"
#include "kafka/requests/response_header.hpp"

namespace kafka {

// Abstract base class of an API response.
class Response {
public:
        virtual ~Response() {}

        // Writes this API response to a byte stream, without the size prefix.
        void write(Writable &writable) const {
                header_.write(writable);
                write_response_body(writable);
        }

        // Sets the header of this response.
        void set_response_header(ResponseHeader header) {
                header_ = std::move(header);
        }

private:
        ResponseHeader header_;

        virtual void write_response_body(Writable &writable) const = 0;
};

}

#endif
