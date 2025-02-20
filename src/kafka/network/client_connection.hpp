#ifndef codecrafters_kafka_network_client_io_channel_hpp
#define codecrafters_kafka_network_client_io_channel_hpp

#include <memory>
#include <unistd.h>
#include <utility>

#include "kafka/requests/request.hpp"
#include "kafka/requests/response.hpp"

namespace kafka {

class ClientConnection {
public:
        explicit ClientConnection(int client_socket) : client_socket_(client_socket) {}

        ~ClientConnection() {
                if (client_socket_ >= 0) {
                        close(client_socket_);
                }
        }

        ClientConnection(const ClientConnection &other) = delete;
        ClientConnection &operator=(const ClientConnection &other) = delete;

        ClientConnection(ClientConnection &&other) noexcept : client_socket_(std::exchange(other.client_socket_, -1)) {}

        ClientConnection &operator=(ClientConnection &&other) noexcept {
                std::swap(client_socket_, other.client_socket_);
                return *this;
        }

        // Receives an API request from this client connection. Throws runtime error if client disconnects.
        std::unique_ptr<Request> receive_request();

        // Sends an API response to this client connection.
        void send_response(std::unique_ptr<Response> response);

private:
        int client_socket_;

        std::int32_t receive_message_size();
        void send_message_size(std::int32_t message_size);
};

}

#endif
