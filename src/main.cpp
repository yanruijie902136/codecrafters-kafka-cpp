#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <vector>

namespace kafka {

static void full_read(int client_socket, void *buffer, std::size_t nbytes) {
    char *p = static_cast<char *>(buffer);
    while (nbytes > 0) {
        ssize_t nr = read(client_socket, p, nbytes);
        if (nr < 0) {
            throw std::system_error(errno, std::system_category(), "read");
        } else if (nr == 0) {
            throw std::runtime_error("incomplete read");
        }
        p += nr;
        nbytes -= nr;
    }
}

static void full_write(int client_socket, const void *buffer, std::size_t nbytes) {
    const char *p = static_cast<const char *>(buffer);
    while (nbytes > 0) {
        ssize_t nw = write(client_socket, p, nbytes);
        if (nw < 0) {
            throw std::system_error(errno, std::system_category(), "write");
        }
        p += nw;
        nbytes -= nw;
    }
}

static int read_int32(int client_socket) {
    int n;
    full_read(client_socket, &n, sizeof(n));
    return htonl(n);
}

static void send_int16(int client_socket, short n) {
    n = htons(n);
    full_write(client_socket, &n, sizeof(n));
}

static void send_int32(int client_socket, int n) {
    n = htonl(n);
    full_write(client_socket, &n, sizeof(n));
}

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

            int message_size = read_int32(client_socket);
            std::vector<char> buffer(message_size);
            full_read(client_socket, buffer.data(), message_size);

            int correlation_id = htonl(*reinterpret_cast<int *>(buffer.data() + 4));
            send_int32(client_socket, 0);
            send_int32(client_socket, correlation_id);
            send_int16(client_socket, 35);

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
