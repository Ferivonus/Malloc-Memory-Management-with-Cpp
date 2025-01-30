#ifndef NAME_MANAGER_H
#define NAME_MANAGER_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <locale>
#include <codecvt>
#include <stdexcept>

class NameManager {
private:
    char* names;
    size_t totalSize;

    void allocateMemory(size_t newSize);

public:
    NameManager();
    ~NameManager();

    void readNamesFromFile(const std::string& filename);
    void printNames() const;
    void clearNames();
};

#endif // NAME_MANAGER_H
