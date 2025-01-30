#include "NameManager.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

const size_t MAX_MEMORY = 1024 * 1024 * 100;  // 100MB memory limit in here, if u try to break the app LOL.

NameManager::NameManager() : names(nullptr), totalSize(0) {}

NameManager::~NameManager() {
    clearNames();
}

void NameManager::allocateMemory(size_t newSize) {
    if (newSize > MAX_MEMORY) {
        std::cerr << "Memory usage exceeded the limit of " << MAX_MEMORY / (static_cast<unsigned long long>(1024) * 1024) << " MB!" << std::endl;
        clearNames();
        throw std::runtime_error("Exceeded memory limit");
    }

    char* temp = static_cast<char*>(realloc(names, newSize));
    if (!temp) {
        std::cerr << "Error allocating " << newSize << " bytes for names. Total allocated memory: " << totalSize << " bytes." << std::endl;
        clearNames();
        throw std::bad_alloc();
    }
    names = temp;
}

void NameManager::readNamesFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("File could not be opened!");
    }

    std::string line;

    while (std::getline(file, line)) {
        size_t nameLen = line.size() + 1; // Directly use UTF-8 encoded line

        // Allocate larger blocks of memory to reduce reallocations
        allocateMemory(totalSize + nameLen);
        std::memcpy(names + totalSize, line.c_str(), nameLen);
        totalSize += nameLen;
    }
}

void NameManager::printNames() const {
    if (!names) {
        std::cerr << "No names stored!" << std::endl;
        return;
    }

    size_t index = 0;
    while (index < totalSize) {
        std::cout << (names + index) << std::endl;
        index += std::strlen(names + index) + 1;
    }
}

void NameManager::clearNames() {
    free(names);
    names = nullptr;
    totalSize = 0;
}
