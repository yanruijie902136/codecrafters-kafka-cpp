#ifndef CODECRAFTERS_KAFKA_NETWORK_SERVER_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_NETWORK_SERVER_HPP_INCLUDED

#include <unistd.h>
#include <utility>

namespace kafka {

// Kafka server that communicates with clients via TCP protocol.
class Server {
public:
    Server();
    Server(Server &&other) noexcept : server_socket_(std::exchange(other.server_socket_, -1)) {}

    ~Server() {
        if (server_socket_ >= 0) {
            close(server_socket_);
        }
    }

    Server &operator=(Server &&other) noexcept {
        std::swap(server_socket_, other.server_socket_);
        return *this;
    }

    // Starts accepting client connections and serving their requests.
    void start();

    Server(const Server &other) = delete;
    Server &operator=(const Server &other) = delete;

private:
    int server_socket_;
};

}

#endif  // CODECRAFTERS_KAFKA_NETWORK_SERVER_HPP_INCLUDED
