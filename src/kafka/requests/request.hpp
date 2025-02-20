#ifndef codecrafters_kafka_requests_request_hpp
#define codecrafters_kafka_requests_request_hpp

#include <memory>

#include "kafka/protocol/readable.hpp"
#include "kafka/requests/request_header.hpp"

namespace kafka {

// Abstract base class of an API request.
class Request {
public:
        virtual ~Request() {}

        // Reads an API request from a byte stream, without the size prefix.
        static std::unique_ptr<Request> read(Readable &readable);

        // The header of this request.
        const RequestHeader &header() const {
                return header_;
        }

private:
        RequestHeader header_;

        // Reads the body of this request.
        virtual void read_request_body(Readable &readable) = 0;
};

}

#endif
