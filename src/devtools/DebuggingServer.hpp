#pragma once

#include <memory>
#include <string>

namespace network {
    class Server;
    class Connection;
}

class Engine;

namespace devtools {
    class DebuggingServer {
    public:
        DebuggingServer(Engine& engine, const std::string& serverString);
        ~DebuggingServer();

        bool update();

        void setClient(network::Connection& client) {
            this->client = &client;
        }
    private:
        Engine& engine;
        network::Server& server;
        network::Connection* client;
    };
}
