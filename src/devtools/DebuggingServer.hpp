#pragma once

#include <memory>
#include <string>

#include "typedefs.hpp"

namespace network {
    class Server;
    class Connection;
    class ReadableConnection;
    class Network;
}

namespace dv {
    class value;
}

class Engine;

namespace devtools {
    class ClientConnection {
    public:
        ClientConnection(network::Network& network, u64id_t connection)
            : network(network), connection(connection) {
        }
        ~ClientConnection();

        std::string read();
        void send(const dv::value& message);
        void sendResponse(const std::string& type);
    private:
        network::Network& network;
        size_t messageLength = 0;
        u64id_t connection;
    };

    class DebuggingServer {
    public:
        DebuggingServer(Engine& engine, const std::string& serverString);
        ~DebuggingServer();

        bool update();

        void setClient(u64id_t client);
    private:
        Engine& engine;
        network::Server& server;
        std::unique_ptr<ClientConnection> connection;

        bool performCommand(const std::string& type, const dv::value& map);
    };
}
