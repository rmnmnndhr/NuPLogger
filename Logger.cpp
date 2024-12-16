#include "Logger.hpp"


/**
 * @brief Die Logger-Klasse bietet Logging-Funktionalität mit Multithreading-Unterstützung.
 * 
 * Diese Klasse implementiert ein Singleton-Design, um sicherzustellen, 
 * dass nur eine Instanz existiert. Logs werden im Hintergrund verarbeitet
 * und bei Bedarf in Dateien geschrieben.
 */
 
/**
 * @brief Gibt die Singleton-Instanz des Loggers zurück.
 *
 * @param filename Der Dateiname, in den die Logs geschrieben werden sollen.
 * @return Referenz zur Singleton-Instanz des Loggers.
 */
Logger& Logger::getInstance(const std::string& filename) {
    static Logger instance(filename);
    return instance;
}

/**
 * @brief Konstruktor der Logger-Klasse.
 *
 * Initialisiert den Logger, öffnet die Datei und startet den Hintergrundthread zur Log-Verarbeitung.
 *
 * @param filename Der Dateiname, in den die Logs geschrieben werden.
 */
Logger::Logger(const std::string& filename)
    : fileHandler(filename), isSuspended(false), stopLogging(false) {
    logThread = std::thread(&Logger::processLogs, this);
    std::cout << "Logger gestartet und bereit." << std::endl;
}

/**
 * @brief Destruktor der Logger-Klasse.
 *
 * Beendet den Hintergrundthread sicher und schreibt alle verbleibenden Logs in die Datei.
 */
Logger::~Logger() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stopLogging = true;
    }
    cv.notify_all();

    if (logThread.joinable()) {
        logThread.join();
    }

    std::cout << "Logger wurde erfolgreich beendet." << std::endl;
}

/**
 * @brief Registriert eine neue Log-Quelle.
 *
 * @param source Der Name der Quelle, die registriert werden soll.
 */
void Logger::registerSource(const std::string& source) {
    std::lock_guard<std::mutex> lock(mtx);
    if (sources.find(source) == sources.end()) {
        sources[source] = true;
        std::cout << "Task registriert: " << source << std::endl;
    }
    else
    {
        std::cout << "Task schon registriert: " << source << std::endl;
    }
}

/**
 * @brief Fügt eine neue Log-Nachricht zur Warteschlange hinzu.
 *
 * @param source Die Quelle der Log-Nachricht.
 * @param data Die eigentlichen Log-Daten.
 */
void Logger::log(const std::string& source, const std::string& data) {
    std::unique_lock<std::mutex> lock(mtx);

    if (isSuspended.load()) {
        std::cerr << "Logger ist pausiert. Log ignoriert: " << source << std::endl;
        return;
    }

    if (sources.find(source) == sources.end()) {
        std::cerr << "Unregistrierte Task: " << source << std::endl;
        return;
    }

    enqueueLog(source, data);
    cv.notify_one();
}

/**
 * @brief Pausiert das Logging.
 *
 * Nach dem Aufruf dieser Methode werden neue Logs ignoriert, 
 * bis die Methode resume() aufgerufen wird.
 */
void Logger::suspend() {
    isSuspended.store(true);
    std::cout << "Logger pausiert." << std::endl;
}

/**
 * @brief Reaktiviert das Logging.
 *
 * Nach dem Aufruf dieser Methode wird das Logging fortgesetzt.
 */
void Logger::resume() {
    isSuspended.store(false);
    cv.notify_all();
    std::cout << "Logger reaktiviert." << std::endl;
}

/**
 * @brief Hintergrundprozess zur Log-Verarbeitung.
 *
 * Diese Methode wird im Hintergrundthread ausgeführt. Sie verarbeitet
 * alle Logs in der Warteschlange und schreibt sie mithilfe des FileHandlers in die Log-Datei.
 */
void Logger::processLogs() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);

        // Warten
        cv.wait(lock, [this] { return !logQueue.empty() || stopLogging.load(); });

        if (stopLogging.load() && logQueue.empty()) {
            break;
        }

        while (!logQueue.empty()) {
            auto logData = logQueue.front();
            logQueue.pop();

            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm* logTime = std::localtime(&now);

            fileHandler.addLog(*logTime, logData.first, logData.second, 0);
        }
    }
}

/**
 * @brief Fügt einen Log-Eintrag zur Warteschlange hinzu.
 *
 * @param source Die Quelle des Logs.
 * @param data Die Log-Daten.
 */
void Logger::enqueueLog(const std::string& source, const std::string& data) {
    logQueue.emplace(source, data);
}
