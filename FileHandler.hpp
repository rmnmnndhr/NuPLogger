// Just for reference, it is already correct

#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>

class FileHandler {
public:
    FileHandler(const std::string& filename);
    ~FileHandler();

    void addLog(const std::tm& logTime, const std::string& source, const std::string& data, int poolID);
    void saveToFile();
    void loadFromFile();

    // Getter for log count
    int getLogCount() const;

private:
    std::string filename;
    std::unordered_map<std::string, std::string> hashmap;
    std::vector<std::string> logs;
    int logLineCount = 0;
    int hashLineCount = 0;

    bool createFileIfNotExists();
    void writeToFile(const std::string& text);
    void addToHashmap(const std::string& hash, const std::string& value);
    std::string generateHash(const std::string& input, size_t length);
};

#endif // FILEHANDLER_HPP
