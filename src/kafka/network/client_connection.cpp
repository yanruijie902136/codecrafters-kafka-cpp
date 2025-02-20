#include "kafka/network/client_connection.hpp"
#include "kafka/utils/bytes_io.hpp"
#include "kafka/utils/syscalls.hpp"

#include <arpa/inet.h>

namespace kafka {

std::unique_ptr<Request> ClientConnection::receive_request() {
        std::int32_t message_size = receive_message_size();
        BytesIO readable(client_socket_, message_size);
        return Request::read(readable);
}

void ClientConnection::send_response(std::unique_ptr<Response> response) {
        BytesIO writable;
        response->write(writable);
        const auto &data = writable.get_value();
        send_message_size(data.size());
        fullwrite(client_socket_, data.data(), data.size());
}

std::int32_t ClientConnection::receive_message_size() {
        std::int32_t message_size;
        fullread(client_socket_, &message_size, sizeof(message_size));
        return ntohl(message_size);
}

void ClientConnection::send_message_size(std::int32_t message_size) {
        message_size = htonl(message_size);
        fullwrite(client_socket_, &message_size, sizeof(message_size));
}

}
