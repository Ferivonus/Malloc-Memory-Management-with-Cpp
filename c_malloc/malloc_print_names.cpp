#ifndef NAME_MANAGER_H
#define NAME_MANAGER_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <locale>
#include <codecvt>
#include <string>

class NameManager {
private:
    char* names;
    size_t totalSize;

    void allocateMemory(size_t newSize) {
        char* temp = static_cast<char*>(realloc(names, newSize));
        if (!temp) {
            std::cerr << "Memory allocation failed!" << std::endl;
            clearNames();
            throw std::bad_alloc();
        }
        names = temp;
    }

public:
    NameManager() : names(nullptr), totalSize(0) {}

    ~NameManager() {
        clearNames();
    }

    void readNamesFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file) {
            throw std::runtime_error("File could not be opened!");
        }

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string line;
        while (std::getline(file, line)) {
            std::wstring wide_line = converter.from_bytes(line);
            size_t nameLen = wide_line.size() + 1;

            char* current_name = static_cast<char*>(malloc(nameLen));
            if (!current_name) {
                throw std::bad_alloc();
            }

            for (size_t i = 0; i < wide_line.size(); ++i) {
                current_name[i] = static_cast<char>(wide_line[i]);
            }
            current_name[wide_line.size()] = '\0';

            allocateMemory(totalSize + nameLen);
            std::memcpy(names + totalSize, current_name, nameLen);
            totalSize += nameLen;
            free(current_name);
        }
    }

    void printNames() const {
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

    void clearNames() {
        free(names);
        names = nullptr;
        totalSize = 0;
    }
};

#endif // NAME_MANAGER_H