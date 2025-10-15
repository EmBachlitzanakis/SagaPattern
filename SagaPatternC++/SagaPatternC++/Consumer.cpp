#include "Consumer.h"
#include <iostream>

#ifdef _WIN32  
#include <winsock2.h> // For timeval on Windows  
#else  
#include <sys/time.h> // For timeval on Unix-like systems  
#endif  

Consumer::Consumer(std::shared_ptr<AMQPConnection> conn) : connection(conn) {}

std::string Consumer::consume(const std::string& queue) {
    amqp_queue_declare_ok_t* r = amqp_queue_declare(
        connection->getConnection(), 1, amqp_cstring_bytes(queue.c_str()),
        0, 1, 0, 0, amqp_empty_table);

    if (!r) {
        std::cerr << "Failed to declare queue: " << queue << std::endl;
        return "failure";
    }

    amqp_basic_consume(connection->getConnection(), 1, amqp_cstring_bytes(queue.c_str()),
        amqp_empty_bytes, 0, 1, 0, amqp_empty_table);

    amqp_rpc_reply_t reply = amqp_get_rpc_reply(connection->getConnection());
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        std::cerr << "Failed to start consuming" << std::endl;
        return "failure";
    }

    amqp_envelope_t envelope;
    amqp_maybe_release_buffers(connection->getConnection());

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    amqp_rpc_reply_t result = amqp_consume_message(connection->getConnection(), &envelope, &timeout, 0);

    if (result.reply_type == AMQP_RESPONSE_NORMAL) {
        std::string message(static_cast<char*>(envelope.message.body.bytes), envelope.message.body.len);
        std::cout << "Consuming from " << queue << ": " << message << std::endl;

        amqp_basic_ack(connection->getConnection(), 1, envelope.delivery_tag, 0);
        amqp_destroy_envelope(&envelope);

        return message;
    }
    else {
        std::cerr << "Failed to consume message or timeout occurred" << std::endl;
        return "failure";
    }
}
