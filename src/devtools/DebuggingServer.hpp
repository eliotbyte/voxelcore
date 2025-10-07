#pragma once

#include <memory>
#include <string>
#include <vector>

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

    enum class BreakpointEventType {
        SET_BREAKPOINT = 1,
        REMOVE_BREAKPOINT,
    };
    struct BreakpointEvent {
        BreakpointEventType type;
        std::string source;
        int line;
    };

    class DebuggingServer {
    public:
        DebuggingServer(Engine& engine, const std::string& serverString);
        ~DebuggingServer();

        bool update();
        void onHitBreakpoint(dv::value&& stackTrace);

        void setClient(u64id_t client);
        std::vector<BreakpointEvent> pullBreakpointEvents();
    private:
        Engine& engine;
        network::Server& server;
        std::unique_ptr<ClientConnection> connection;
        std::vector<BreakpointEvent> breakpointEvents;

        bool performCommand(const std::string& type, const dv::value& map);
    };
}
