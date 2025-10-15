#pragma once

#include <memory>
#include <string>
#include "AMQPConnection.h"

class Producer {
private:
    std::shared_ptr<AMQPConnection> connection;

public:
    Producer(std::shared_ptr<AMQPConnection> conn);
    void publish(const std::string& queue, const std::string& message);
};
