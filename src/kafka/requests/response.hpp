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
                write_response_header(writable);
                write_response_body(writable);
        }

        // Sets the header of this response.
        void set_response_header(ResponseHeader header) {
                header_ = std::move(header);
        }

protected:
        ResponseHeader header_;

private:
        virtual void write_response_header(Writable &writable) const {
                header_.write(writable, 1);
        }

        virtual void write_response_body(Writable &writable) const = 0;
};

}

#endif
