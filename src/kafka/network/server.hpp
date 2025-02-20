#ifndef codecrafters_kafka_network_server_hpp
#define codecrafters_kafka_network_server_hpp

#include "kafka/network/client_connection.hpp"

namespace kafka {

class Server {
public:
        Server() : server_socket_(create_server_socket()) {}

        // Starts accepting client connections and handling their requests.
        void start();

private:
        int server_socket_;

        // Creates server socket via socket(2), bind(2), and listen(2).
        static int create_server_socket();

        // Thread function for serving a client connection.
        static void serve_client_connection(ClientConnection connection);
};

}

#endif
