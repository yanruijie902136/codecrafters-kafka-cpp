#include "kafka/network/server.hpp"
#include "kafka/message/headers.hpp"
#include "kafka/network/client.hpp"
#include "kafka/protocol/constants.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>
#include <thread>

namespace kafka {

Server::Server() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        throw std::system_error(errno, std::system_category(), "socket");
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors.
    const int reuse = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        throw std::system_error(errno, std::system_category(), "setsockopt");
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(9092);
    if (bind(server_socket_, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
        throw std::system_error(errno, std::system_category(), "bind");
    }

    const int backlog = 5;
    if (listen(server_socket_, backlog) < 0) {
        throw std::system_error(errno, std::system_category(), "listen");
    }
}

static void serve_client(Client client) {
    for ( ; ; ) {
        auto [request_header, request] = client.read_request();

        ApiVersionsResponse response;
        response.throttle_time_ms() = 0;
        if (request_header.request_api_version() != 4) {
            response.error_code() = ErrorCode::UNSUPPORTED_VERSION;
        } else {
            response.error_code() = ErrorCode::NONE;
            response.api_keys().emplace_back(ApiKey::API_VERSIONS, 0, 4);
        }

        client.write_response(ResponseHeader(request_header.correlation_id()), response);
    }
}

void Server::start() {
    for ( ; ; ) {
        int client_socket = accept(server_socket_, nullptr, nullptr);
        if (client_socket < 0) {
            if (errno == ECONNABORTED || errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::system_category(), "accept");
        }

        Client client(client_socket);
        std::thread t(serve_client, std::move(client));
        t.detach();
    }
}

}
