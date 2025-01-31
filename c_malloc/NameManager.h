#ifndef NAMEMANAGER_H
#define NAMEMANAGER_H

#include <string>

class NameManager {
private:
    char* names;
    size_t totalSize;

    void allocateMemory(size_t newSize);
    void clearNames();

public:
    NameManager();
    ~NameManager();

    void readNamesFromFile(const std::string& filename);
    void printNames() const;
    size_t getTotalSize() const;
    std::string getNameAt(size_t index) const;
    void removeNameAt(size_t index);
    void clearAllNames();
    size_t countNames() const;
    void saveNamesToFile(const std::string& filename) const;
};

#endif // NAMEMANAGER_H
