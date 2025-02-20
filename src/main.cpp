#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <err.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

static int create_server_socket() {
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

static void handle_client(int client_socket) {
        for ( ; ; ) {

        }
}

int main(int argc, char *argv[]) {
        int server_socket = create_server_socket();
        for ( ; ; ) {
                int client_socket = accept(server_socket, nullptr, nullptr);
                if (client_socket < 0) {
                        err(EXIT_FAILURE, "accept");
                }

                std::thread thread(handle_client, client_socket);
                thread.detach();
        }
}
