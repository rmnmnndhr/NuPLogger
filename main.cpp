#include <iostream>
#include <fstream>
#include <sstream>
#include "Logger.hpp"

std::vector<std::string> splitStringMain(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void task3(Logger &logger) {
    logger.log("task3", "Task3 log entry.");
}

void task2(Logger &logger) {
    logger.log("task2", "Task2 log entry.");
}

int main() {
    int Programmtype;
    std::cout << "Gib '1' fuer den Loggertest und '2' fuer die Dateiausgabe ein:" << std::endl;
    std::cin >> Programmtype;
    if (Programmtype == 1) {
        Logger &logger = Logger::getInstance("log.txt");


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
    } else if (Programmtype == 2) {
        std::ifstream file("log.txt");
        if (!file.is_open()) {
            std::cerr << "Could not open the file!" << std::endl;
            return 0;
        }
        int hashLineCount, logLineCount;
        std::unordered_map<std::string, std::string> hashmap;
        std::vector<std::vector<std::string>> logs;

        file >> hashLineCount >> logLineCount;

        std::vector<std::string> hashValues;
        for (size_t i = 0; i < hashLineCount; ++i) {
            std::string hashValuePair;
            file >> hashValuePair;
            hashValues = splitStringMain(hashValuePair, ';');
            std::string val;
            std::getline(file, val);
            hashmap[hashValues[0]] = hashValues[1] + val;
        }

        std::vector<std::string> logValues;
        for (size_t i = 0; i < logLineCount; ++i) {
            std::string log;
            std::getline(file, log);
            logValues = splitStringMain(log, ';');
            logs.push_back(logValues);
        }
        const int pauseInterval = 15;
        int count = 0;
        for (const auto &element: logs) {
            std::cout << "Time:" << element[0] << "; Author:" << element[1] << "; Content:" << hashmap[element[2]]
                      << std::endl;
            count++;
            if (count % pauseInterval == 0) {
                std::cout << "\nDruecke Enter, um fortzufahren...";
                std::cin.ignore();
                std::cin.get();
            }
        }
    }
    return 0;
}
