#include "DebuggingServer.hpp"

#include "engine/Engine.hpp"
#include "network/Network.hpp"
#include "debug/Logger.hpp"
#include "coders/json.hpp"

using namespace devtools;

static debug::Logger logger("debug-server");

ClientConnection::~ClientConnection() {
    if (auto connection = dynamic_cast<network::ReadableConnection*>(
        network.getConnection(this->connection, true)
    )) {
        connection->close();
    }
}

std::string ClientConnection::read() {
    auto connection = dynamic_cast<network::ReadableConnection*>(
        network.getConnection(this->connection, true)
    );
    if (connection == nullptr) {
        return "";
    }
    if (messageLength == 0) {
        if (connection->available() >= sizeof(int32_t)) {
            int32_t length = 0;
            connection->recv(reinterpret_cast<char*>(&length), sizeof(int32_t));
            if (length <= 0) {
                logger.error() << "invalid message length " << length;
            } else {
                messageLength = length;
            }
        }
    } else if (connection->available() >= messageLength) {
        std::string string(messageLength, 0);
        connection->recv(string.data(), messageLength);
        return string;
    }
    return "";
}

void ClientConnection::send(const dv::value& object) {
    auto connection = dynamic_cast<network::ReadableConnection*>(
        network.getConnection(this->connection, true)
    );
    if (connection == nullptr) {
        return;
    }
    auto message = json::stringify(object, false);
    int32_t length = message.length();
    connection->send(reinterpret_cast<char*>(&length), sizeof(int32_t));
    connection->send(message.data(), length);
}

void ClientConnection::sendResponse(const std::string& type) {
    send(dv::object({{"type", type}}));
}

static network::Server& create_tcp_server(
    DebuggingServer& dbgServer, Engine& engine, int port
) {
    auto& network = engine.getNetwork();
    u64id_t serverId = network.openTcpServer(
        port,
        [&network, &dbgServer](u64id_t sid, u64id_t id) {
            auto& connection = dynamic_cast<network::ReadableConnection&>(
                *network.getConnection(id, true)
            );
            connection.setPrivate(true);
            logger.info() << "connected client " << id << ": "
                          << connection.getAddress() << ":"
                          << connection.getPort();
            dbgServer.setClient(id);
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
      connection(nullptr) {
}

DebuggingServer::~DebuggingServer() {
    logger.info() << "stopping debugging server";
    server.close();
}


bool DebuggingServer::update() {
    if (connection == nullptr) {
        return true;
    }
    std::string message = connection->read();
    if (message.empty()) {
        return true;
    }
    logger.debug() << "received: " << message;
    try {
        auto obj = json::parse(message);
        if (!obj.has("type")) {
            logger.error() << "missing message type";
            return true;
        }
        const auto& type = obj["type"].asString();
        return performCommand(type, obj);
    } catch (const std::runtime_error& err) {
        logger.error() << "could not to parse message: " << err.what();
    }
    return true;
}

bool DebuggingServer::performCommand(
    const std::string& type, const dv::value& map
) {
    if (type == "terminate") {
        engine.quit();
        connection->sendResponse("success");
    } else if (type == "detach") {
        connection->sendResponse("success");
        connection.reset();
        return false;
    } else {
        logger.error() << "unsupported command '" << type << "'";
    }
    return true;
}

void DebuggingServer::setClient(u64id_t client) {
    this->connection =
        std::make_unique<ClientConnection>(engine.getNetwork(), client);
}
