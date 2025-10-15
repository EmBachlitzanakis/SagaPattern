#pragma once

#include <memory>
#include <string>
#include "AMQPConnection.h"

class Consumer {
private:
    std::shared_ptr<AMQPConnection> connection;

public:
    Consumer(std::shared_ptr<AMQPConnection> conn);
    std::string consume(const std::string& queue = "saga_response");
};
