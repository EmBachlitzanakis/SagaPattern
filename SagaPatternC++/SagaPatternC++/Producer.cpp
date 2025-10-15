#include "Producer.h"
#include <iostream>

Producer::Producer(std::shared_ptr<AMQPConnection> conn) : connection(conn) {}

void Producer::publish(const std::string& queue, const std::string& message) {
    amqp_queue_declare_ok_t* r = amqp_queue_declare(
        connection->getConnection(), 1, amqp_cstring_bytes(queue.c_str()),
        0, 1, 0, 0, amqp_empty_table);

    if (!r) {
        std::cerr << "Failed to declare queue: " << queue << std::endl;
        return;
    }

    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2;

    int result = amqp_basic_publish(
        connection->getConnection(),
        1,
        amqp_cstring_bytes(""),
        amqp_cstring_bytes(queue.c_str()),
        0,
        0,
        &props,
        amqp_cstring_bytes(message.c_str()));

    if (result) {
        std::cerr << "Failed to publish message to queue: " << queue << std::endl;
    }
    else {
        std::cout << "Publishing to " << queue << ": " << message << std::endl;
    }
}
