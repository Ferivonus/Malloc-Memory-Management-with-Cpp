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

    // Allocate memory using malloc
    char* temp = static_cast<char*>(malloc(newSize));
    if (!temp) {
        std::cerr << "Error allocating " << newSize << " bytes for names. Total allocated memory: " << totalSize << " bytes." << std::endl;
        clearNames();
        throw std::bad_alloc();
    }

    // Copy previous data to new memory if names already exists
    if (names) {
        std::memcpy(temp, names, totalSize);
        free(names);  // Free old memory block
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
        size_t nameLen = line.size() + 1;  // Include the null terminator

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

size_t NameManager::getTotalSize() const {
    return totalSize;
}

std::string NameManager::getNameAt(size_t index) const {
    if (index >= totalSize) {
        throw std::out_of_range("Index is out of range.");
    }

    size_t currentIndex = 0;
    size_t nameIndex = 0;

    while (currentIndex < totalSize) {
        if (nameIndex == index) {
            return std::string(names + currentIndex);
        }
        currentIndex += std::strlen(names + currentIndex) + 1;
        ++nameIndex;
    }

    throw std::out_of_range("Index is out of range.");
}

void NameManager::removeNameAt(size_t index) {
    if (index >= totalSize) {
        throw std::out_of_range("Index is out of range.");
    }

    size_t currentIndex = 0;
    size_t nameIndex = 0;

    while (currentIndex < totalSize) {
        if (nameIndex == index) {
            size_t nameLen = std::strlen(names + currentIndex) + 1;
            std::memmove(names + currentIndex, names + currentIndex + nameLen, totalSize - currentIndex - nameLen);
            totalSize -= nameLen;
            return;
        }
        currentIndex += std::strlen(names + currentIndex) + 1;
        ++nameIndex;
    }

    throw std::out_of_range("Index is out of range.");
}

void NameManager::clearAllNames() {
    clearNames();
}

size_t NameManager::countNames() const {
    size_t count = 0;
    size_t index = 0;
    while (index < totalSize) {
        ++count;
        index += std::strlen(names + index) + 1;
    }
    return count;
}

void NameManager::saveNamesToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file for writing.");
    }

    size_t index = 0;
    while (index < totalSize) {
        std::string name(names + index);
        file << name << std::endl;
        index += name.size() + 1;
    }
}
