#include "kafka/network/server.hpp"
#include "kafka/handlers/request_handler.hpp"
#include "kafka/network/client_connection.hpp"
#include "kafka/requests/request.hpp"
#include "kafka/requests/response.hpp"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <err.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <utility>

namespace kafka {

int Server::create_server_socket() {
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0) {
                err(EXIT_FAILURE, "socket");
        }

        const int reuse = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
                err(EXIT_FAILURE, "setsockopt");
        }

        sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(9092);

        if (bind(server_socket, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
                err(EXIT_FAILURE, "bind");
        }

        const int connection_backlog = 5;
        if (listen(server_socket, connection_backlog) < 0) {
                err(EXIT_FAILURE, "listen");
        }

        return server_socket;
}

void Server::start() {
        for ( ; ; ) {
                int client_socket = accept(server_socket_, nullptr, nullptr);
                if (client_socket < 0) {
                        err(EXIT_FAILURE, "accept");
                }

                ClientConnection connection(client_socket);
                std::thread thread(serve_client_connection, std::move(connection));
                thread.detach();
        }
}

void Server::serve_client_connection(ClientConnection connection) {
        std::unique_ptr<Request> request;
        std::unique_ptr<RequestHandler> request_handler;
        std::unique_ptr<Response> response;

        for ( ; ; ) {
                try {
                        request = connection.receive_request();
                } catch (...) {
                        return;
                }
                request_handler = choose_request_handler(request.get());
                response = request_handler->handle_request(request.get());
                connection.send_response(std::move(response));
        }
}

}
