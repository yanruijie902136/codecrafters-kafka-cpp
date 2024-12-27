#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

namespace kafka {

class Server {
public:
    Server() {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0) {
            throw std::system_error(errno, std::system_category(), "socket");
        }

        // Since the tester restarts your program quite often, setting SO_REUSEADDR ensures that
        // we don't run into 'Address already in use' errors.
        int reuse = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            throw std::system_error(errno, std::system_category(), "setsockopt");
        }

        sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(9092);
        if (bind(server_socket, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
            throw std::system_error(errno, std::system_category(), "bind");
        }

        int backlog = 5;
        if (listen(server_socket, backlog) < 0) {
            throw std::system_error(errno, std::system_category(), "listen");
        }
    }

    void start() {
        for ( ; ; ) {
            int client_socket = accept(server_socket, nullptr, nullptr);
            if (client_socket < 0) {
                if (errno == ECONNABORTED || errno == EINTR) {
                    continue;
                }
                throw std::system_error(errno, std::system_category(), "accept");
            }

            close(client_socket);
        }
    }

private:
    int server_socket;
};

}

int main(int argc, char *argv[]) {
    kafka::Server server;
    server.start();
}
