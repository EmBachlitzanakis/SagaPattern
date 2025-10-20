# Saga Pattern C++

A C++ implementation of the Saga pattern for managing distributed transactions with compensating actions, using RabbitMQ for message-based coordination.

## Overview

This project demonstrates the **Saga pattern** - a design pattern for managing data consistency across microservices in distributed systems. When a step fails, the saga automatically executes compensating transactions to undo previously completed steps.

### Example Use Case

The implementation simulates a **travel booking system** with two steps:
1. **Flight Reservation** - Books a flight (70% success rate)
2. **Hotel Reservation** - Books a hotel (75% success rate)

If any step fails, the saga automatically cancels all previously successful bookings in reverse order.

## Features

 **Sequential step execution** with automatic rollback
 **Compensation logic** for each saga step
 **RabbitMQ integration** for message-based coordination
 **RAII resource management** for connection and channel lifecycle
 **Exception handling** with proper cleanup
 **Extensible design** - easy to add new saga steps

## Prerequisites

- **C++14** compatible compiler (MSVC, GCC, Clang)
- **RabbitMQ server** running on localhost:5672
- **RabbitMQ-C library** (librabbitmq)
