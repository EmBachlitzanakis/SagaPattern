#pragma once

#include <string>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_framing.h>

class AMQPConnection {
private:
    amqp_connection_state_t conn;
    amqp_socket_t* socket;
    std::string hostname;
    int port;
    std::string vhost;
    std::string username;
    std::string password;

public:
    AMQPConnection(const std::string& host = "localhost", int p = 5672,
        const std::string& vh = "/", const std::string& user = "guest",
        const std::string& pass = "guest");
    ~AMQPConnection();

    amqp_connection_state_t getConnection() const;
};
