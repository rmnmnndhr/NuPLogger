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
double avg;
void threadFunction(Logger &logger, const std::string taskid, const std::string &data) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::ostringstream oss;
    oss << "Daten von Thread " << taskid << ": " << data;
    int counter = 0;
    std::vector<long long> durations;
    while (counter < 200) {
        auto start = std::chrono::high_resolution_clock::now();
        logger.log(taskid, data);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        durations.push_back(duration);
        counter++;
        //std::this_thread::sleep_for(std::chrono::milliseconds (10));
        if (counter == 15 && taskid == "task10") {
            logger.suspend();
        } else if (counter == 25 && taskid == "task10") {
            logger.resume();
        }
    }
    long long sum = 0;
    for (long long d: durations) {
        sum += d;
    }
    double averageDuration = static_cast<double>(sum) / durations.size();
    std::cout << averageDuration << std::endl;
    avg += averageDuration;
}

int main() {
    int Programmtype;
    std::cout << "Gib '1' fuer den Loggertest und '2' fuer die Dateiausgabe ein:" << std::endl;
    std::cin >> Programmtype;
    if (Programmtype == 1) {
        Logger &logger = Logger::getInstance("log.txt");


        int numberOfThreads{20};
        std::vector<std::thread> threads;
        for (int i = 10; i < numberOfThreads + 10; ++i) {
            logger.registerSource("task" + std::to_string(i));
            threads.emplace_back(threadFunction, std::ref(logger), "task" + std::to_string(i),
                                 "Task" + std::to_string(i) + " log entry.");
        }


        //logger.suspend();  // Pause logging
        //std::this_thread::sleep_for(std::chrono::milliseconds (110));
        //logger.resume();   // Resume logging
        std::cout << "end" << std::endl;
        for (auto &t: threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        std::cout << "joined" << std::endl;
        std::cout << "Total Average:" << avg/numberOfThreads << std::endl;
        double averageDuration = static_cast<double>(logger.logTimeMicro) / logger.logCount;
        std::cout << "Log Average:" << averageDuration << std::endl;
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
