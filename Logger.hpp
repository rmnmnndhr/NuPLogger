#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "FileHandler.hpp"
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>


class Logger {
public:
    static Logger& getInstance(const std::string& filename);
    void registerSource(const std::string& source);
    void log(const std::string& source, const std::string& data);
    void suspend();
    void resume();
    long long logTimeMicro{0};
    int logCount{0};
    ~Logger();

private:
    Logger(const std::string& filename);
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void processLogs();
    void enqueueLog(const std::string& source, const std::string& data);

    FileHandler fileHandler;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::pair<std::string, std::string>> logQueue; 
    std::unordered_map<std::string, bool> sources;

    std::atomic<bool> isSuspended;
    std::atomic<bool> stopLogging;
    std::thread logThread;
};

#endif // LOGGER_HPP
