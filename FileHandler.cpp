#include "FileHandler.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>

FileHandler::FileHandler(const std::string& filename) {
    this->filename = filename;
    loadFromFile();
}

FileHandler::~FileHandler() {
    saveToFile();
}


/**
 *
 * @return
 */
bool FileHandler::createFileIfNotExists() {
    std::ifstream file(filename);
    if (!file.good()) {
        std::ofstream newFile(filename);
        if (!newFile.is_open()) {
            std::cerr << "Could not create the file!" << std::endl;
            return false;
        }
        // Füllt die Datei mit den Zeilenzählern bei 0.
        newFile << "0\n0\n";
        newFile.close();
    }
    return true;
}


/**
 *
 * @param str
 * @param delimiter
 * @return
 */
std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


/**
 * Lädt die gespeicherten Zeilen Zähler und die Hashmap aus der Datei aus.
 */
void FileHandler::loadFromFile() {
    createFileIfNotExists();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }

    file >> hashLineCount >> logLineCount;
    hashmap.clear();
    std::vector<std::string> hashValues;

    for (size_t i = 0; i < hashLineCount; ++i) {
        std::string hashValuePair;
        file >> hashValuePair;
        hashValues = splitString(hashValuePair, ';');
        std::string val;
        std::getline(file, val);
        hashmap[hashValues[0]] = hashValues[1] + val;
    }

    logs.clear();
    for (size_t i = 0; i < logLineCount; ++i) {
        std::string log;
        std::getline(file, log);
        logs.push_back(log);
    }

    file.close();
}


/**
 *
 */
void FileHandler::saveToFile() {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }

    file << hashLineCount << std::endl;
    file << logLineCount << std::endl;

    for (const auto& pair : hashmap) {
        file << pair.first << ";" << pair.second << std::endl;
    }

    for (const auto& log : logs) {
        file << log << std::endl;
    }

    file.close();
}


/**
 * Setzt die Log Parameter zusammen und incrementiert den Counter.
 * @param logTime Der Zeitpunkt zu dem der Log erstellt wurde.
 * @param source Identifikator des schreibenden Programmabschnittes.
 * @param data Die Information die gehasht und gespeichert werden soll.
 */

// geaendert 'const std::tm logTime' to 'const std::tm& logTime'
void FileHandler::addLog(const std::tm& logTime, const std::string& source, const std::string& data) {
    std::string hash = generateHash(data, 50);
    addToHashmap(hash, data);

    std::stringstream logEntry;
    logEntry << logTime.tm_mday << "." << logTime.tm_mon + 1 << "." << logTime.tm_year + 1900 << " "
             << logTime.tm_hour << ":" << logTime.tm_min << ":" << logTime.tm_sec << ";"
             << source << ";" << hash;

    writeToFile(logEntry.str());
    logLineCount++;
    logs.push_back(logEntry.str());
}

/**
 *
 * @param hash
 * @param value
 */
void FileHandler::addToHashmap(const std::string& hash, const std::string& value) {
    hashmap[hash] = value;
    hashLineCount = hashmap.size();
}

/**
 * Schreibt einen String an das Ende der Datei.
 * @param text
 */
void FileHandler::writeToFile(const std::string& text) {
    std::ofstream outFile(filename, std::ios::app);
    if (outFile.is_open()) {
        outFile << text << std::endl;
        outFile.close();
    } else {
        std::cerr << "Error opening file!" << std::endl;
    }
}



std::string FileHandler::generateHash(const std::string& input, size_t length) {
    std::hash<std::string> hash_fn;
    size_t hash = hash_fn(input);

    std::string hashStr = std::to_string(hash);
    if (length > hashStr.size()) {
        length = hashStr.size();
    }
    return hashStr.substr(0, length);
}

// Getter fuer log count
int FileHandler::getLogCount() const {
    return logLineCount;
}
