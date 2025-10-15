// SagaPatternC++.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include <ctime>
#include "AMQPConnection.h"
#include "Producer.h"
#include "Consumer.h"


class SagaStep {
public:
    virtual void execute() = 0;
    virtual void compensate() = 0;
    virtual ~SagaStep() = default;
};

class FlightReservationStep : public SagaStep {
private:
    Producer& producer;

public:
    FlightReservationStep(Producer& prod) : producer(prod) {}

    void execute() override {
        try {
            std::cout << "Reserving flight...\n";
            bool reservationSuccess = (rand() % 10) < 7;
            if (reservationSuccess) {
                producer.publish("saga_response", "success");
                std::cout << "Flight reservation successful\n";
            }
            else {
                producer.publish("saga_response", "failure");
                std::cout << "Flight reservation failed\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during flight reservation: " << e.what() << std::endl;
            producer.publish("saga_response", "failure");
        }
    }

    void compensate() override {
        try {
            std::cout << "Cancelling flight...\n";
            bool cancellationSuccess = (rand() % 10) < 9;
            if (cancellationSuccess) {
                producer.publish("saga_response", "cancelled");
                std::cout << "Flight cancellation successful\n";
            }
            else {
                producer.publish("saga_response", "compensation_failed");
                std::cout << "Flight cancellation failed\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during compensation: " << e.what() << std::endl;
            producer.publish("saga_response", "compensation_failed");
        }
    }
};

class HotelReservationStep : public SagaStep {
private:
    Producer& producer;

public:
    HotelReservationStep(Producer& prod) : producer(prod) {}

    void execute() override {
        try {
            std::cout << "Reserving hotel...\n";
            bool reservationSuccess = (rand() % 10) < 7.5;
            if (reservationSuccess) {
                producer.publish("saga_response", "success");
                std::cout << "Hotel reservation successful\n";
            }
            else {
                producer.publish("saga_response", "failure");
                std::cout << "Hotel reservation failed\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during hotel reservation: " << e.what() << std::endl;
            producer.publish("saga_response", "failure");
        }
    }

    void compensate() override {
        try {
            std::cout << "Cancelling hotel reservation...\n";
            bool cancellationSuccess = (rand() % 10) < 8.5;
            if (cancellationSuccess) {
                producer.publish("saga_response", "cancelled");
                std::cout << "Hotel cancellation successful\n";
            }
            else {
                producer.publish("saga_response", "compensation_failed");
                std::cout << "Hotel cancellation failed\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during hotel compensation: " << e.what() << std::endl;
            producer.publish("saga_response", "compensation_failed");
        }
    }
};

class SagaManager {
private:
    Consumer& consumer;
    std::vector<std::shared_ptr<SagaStep>> steps;
    std::vector<std::shared_ptr<SagaStep>> executedSteps;

public:
    SagaManager(Consumer& cons) : consumer(cons) {}

    void addStep(std::shared_ptr<SagaStep> step) {
        steps.push_back(step);
    }

    void execute() {
        std::cout << "Starting travel booking saga...\n";
        std::cout << "=========================================\n";

        for (size_t i = 0; i < steps.size(); ++i) {
            std::cout << "\n--- Step " << (i + 1) << " of " << steps.size() << " ---\n";
            steps[i]->execute();
            executedSteps.push_back(steps[i]);

            std::string message = consumer.consume();
            if (message == "failure") {
                std::cout << "\n Step failed. Starting compensation process...\n";
                std::cout << "=========================================\n";
                compensateExecutedSteps();
                return;
            }
        }
        std::cout << "\nTravel booking saga completed successfully!\n";
        std::cout << "All services have been booked successfully.\n";
    }

private:
    void compensateExecutedSteps() {
        for (auto it = executedSteps.rbegin(); it != executedSteps.rend(); ++it) {
            std::cout << "\nCompensating previous step...\n";
            (*it)->compensate();

            std::string compensationResult = consumer.consume();
            if (compensationResult == "compensation_failed") {
                std::cerr << "Compensation failed for a step! Manual intervention required." << std::endl;
            }
        }
        std::cout << "\nTravel booking saga failed and compensated.\n";
    }
};

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    try {
        auto connection = std::make_shared<AMQPConnection>("localhost", 5672, "/", "guest", "guest");
        Producer producer(connection);
        Consumer consumer(connection);

        SagaManager sagaManager(consumer);

        sagaManager.addStep(std::make_shared<FlightReservationStep>(producer));
        sagaManager.addStep(std::make_shared<HotelReservationStep>(producer));

        sagaManager.execute();

        std::cout << "\nSaga execution completed.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
