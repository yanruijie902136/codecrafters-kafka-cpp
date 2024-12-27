#ifndef CODECRAFTERS_KAFKA_NETWORK_CLIENT_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_NETWORK_CLIENT_HPP_INCLUDED

#include "kafka/message/api_versions.hpp"
#include "kafka/message/headers.hpp"
#include "kafka/protocol/file_descriptor.hpp"
#include "kafka/protocol/readable_buffer.hpp"
#include "kafka/protocol/writable_buffer.hpp"

namespace kafka {

// Kafka client connection.
class Client {
public:
    explicit Client(int client_socket) : client_fd_(client_socket) {}

    // Reads a request from this client connection.
    std::pair<RequestHeader, ApiVersionsRequest> read_request() {
        ReadableBuffer rb(read_bytes(client_fd_));
        RequestHeader request_header;
        request_header.read(rb);
        ApiVersionsRequest request;
        request.read(rb);
        return {request_header, request};
    }

    // Writes a response to this client connection.
    void write_response(const ResponseHeader &response_header, const ApiVersionsResponse &response) {
        WritableBuffer wb;
        response_header.write(wb);
        response.write(wb);
        write_bytes(client_fd_, wb.buffer());
    }

private:
    FileDescriptor client_fd_;
};

}

#endif  // CODECRAFTERS_KAFKA_NETWORK_CLIENT_HPP_INCLUDED
