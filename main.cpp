#include <iostream>
#include "Logger.hpp"

void task3(Logger& logger) {
    logger.log("task3", "Task3 log entry.");
}

void task2(Logger& logger) {
    logger.log("task2", "Task2 log entry.");
}

int main() {
    Logger& logger = Logger::getInstance("log.txt");


    logger.registerSource("task3");
    logger.registerSource("task3");

    for (int i = 0; i < 5; ++i) {
        task3(logger); 
        task2(logger);  
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));  

    logger.suspend();  // Pause logging
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    task3(logger);
    task3(logger);
    logger.resume();   // Resume logging

    return 0;
}
