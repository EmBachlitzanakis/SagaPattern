#include "AMQPConnection.h"
#include <stdexcept>

AMQPConnection::AMQPConnection(const std::string& host, int p,
    const std::string& vh, const std::string& user,
    const std::string& pass)
    : hostname(host), port(p), vhost(vh), username(user), password(pass), socket(nullptr) {

    conn = amqp_new_connection();
    socket = amqp_tcp_socket_new(conn);

    if (!socket) {
        throw std::runtime_error("Failed to create TCP socket");
    }

    int status = amqp_socket_open(socket, hostname.c_str(), port);
    if (status) {
        throw std::runtime_error("Failed to open TCP socket");
    }

    amqp_rpc_reply_t r = amqp_login(conn, vhost.c_str(), 0, 131072, 0,
        AMQP_SASL_METHOD_PLAIN, username.c_str(), password.c_str());
    if (r.reply_type != AMQP_RESPONSE_NORMAL) {
        throw std::runtime_error("Failed to login");
    }

    amqp_channel_open(conn, 1);
    r = amqp_get_rpc_reply(conn);
    if (r.reply_type != AMQP_RESPONSE_NORMAL) {
        throw std::runtime_error("Failed to open channel");
    }
}

AMQPConnection::~AMQPConnection() {
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
}

amqp_connection_state_t AMQPConnection::getConnection() const {
    return conn;
}
