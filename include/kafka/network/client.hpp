#ifndef CODECRAFTERS_KAFKA_NETWORK_CLIENT_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_NETWORK_CLIENT_HPP_INCLUDED

#include "kafka/message/messages.hpp"
#include "kafka/protocol/file_descriptor.hpp"

namespace kafka {

// Kafka client connection.
class Client {
public:
    explicit Client(int client_socket) : client_fd_(client_socket) {}

    // Reads a `RequestMessage` from this client connection.
    RequestMessage read_request() {
        RequestMessage request_message;
        request_message.read(client_fd_);
        return request_message;
    }

    // Writes a `ResponseMessage` to this client connection.
    void write_response(const ResponseMessage &response_message) {
        response_message.write(client_fd_);
    }

private:
    FileDescriptor client_fd_;
};

}

#endif  // CODECRAFTERS_KAFKA_NETWORK_CLIENT_HPP_INCLUDED
