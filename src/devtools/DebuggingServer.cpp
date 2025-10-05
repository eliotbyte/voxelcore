#include "DebuggingServer.hpp"

#include "engine/Engine.hpp"
#include "network/Network.hpp"
#include "debug/Logger.hpp"

using namespace devtools;

static debug::Logger logger("debug-server");

static network::Server& create_tcp_server(
    DebuggingServer& dbgServer, Engine& engine, int port
) {
    auto& network = engine.getNetwork();
    u64id_t serverId = network.openTcpServer(
        port,
        [&network, &dbgServer](u64id_t sid, u64id_t id) {
            auto& connection = *network.getConnection(id, true);
            connection.setPrivate(true);
            logger.info() << "connected client " << id << ": "
                          << connection.getAddress() << ":"
                          << connection.getPort();
            dbgServer.setClient(connection);
        }
    );
    auto& server = *network.getServer(serverId, true);
    server.setPrivate(true);

    auto& tcpServer = dynamic_cast<network::TcpServer&>(server);
    tcpServer.setMaxClientsConnected(1);

    logger.info() << "tcp debugging server open at port " << server.getPort();

    return tcpServer;
}

static network::Server& create_server(
    DebuggingServer& dbgServer, Engine& engine, const std::string& serverString
) {
    logger.info() << "starting debugging server";

    size_t sepPos = serverString.find(':');
    if (sepPos == std::string::npos) {
        throw std::runtime_error("invalid debugging server configuration string");
    }
    auto transport = serverString.substr(0, sepPos);
    if (transport == "tcp") {
        int port;
        try {
            port = std::stoi(serverString.substr(sepPos + 1));
        } catch (const std::exception& err) {
            throw std::runtime_error("invalid tcp port");
        }
        return create_tcp_server(dbgServer, engine, port);
    } else {
        throw std::runtime_error(
            "unsupported debugging server transport '" + transport + "'"
        );
    }
}

DebuggingServer::DebuggingServer(
    Engine& engine, const std::string& serverString
)
    : engine(engine),
      server(create_server(*this, engine, serverString)),
      client(nullptr) {
}

DebuggingServer::~DebuggingServer() {
    logger.info() << "stopping debugging server";
}


bool DebuggingServer::update() {
    return false;
}
