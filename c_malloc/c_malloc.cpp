#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <locale>
#include <codecvt>

// Function to calculate the sum of integers from a file
static int integer_sum(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename.txt>" << std::endl;
        return -1;
    }

    std::ifstream file(argv[1]); // Open file
    if (!file) {
        std::cout << "File could not be opened!" << std::endl;
        return -1;
    }

    int* ptr = nullptr;
    int num, count = 0, sum = 0;

    while (file >> num) {
        int* temp = (int*)realloc(ptr, (static_cast<unsigned long long>(count) + 1) * sizeof(int));
        if (temp == nullptr) {
            std::cout << "Memory allocation failed!" << std::endl;
            free(ptr);
            file.close();
            return -1;
        }
        ptr = temp;
        ptr[count] = num;
        sum += num;
        count++;
    }

    file.close();
    std::cout << "Total: " << sum << std::endl;
    free(ptr);
    return 0;
}

// Function to read names from a UTF-8 encoded file and concatenate them
static int names_function(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename.txt>" << std::endl;
        return -1;
    }

    std::ifstream file(argv[1], std::ios::in | std::ios::binary);
    if (!file) {
        std::cout << "File could not be opened!" << std::endl;
        return -1;
    }

    char* ptr = nullptr;
    size_t count = 0;
    char* current_name = nullptr;
    std::string line;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    while (std::getline(file, line)) {
        std::wstring wide_line = converter.from_bytes(line);
        current_name = (char*)malloc(wide_line.size() + 1);
        if (current_name == nullptr) {
            std::cout << "Memory allocation failed!" << std::endl;
            free(ptr);
            file.close();
            return -1;
        }

        size_t len = wide_line.size();
        for (size_t i = 0; i < len; ++i) {
            current_name[i] = static_cast<char>(wide_line[i]);
        }
        current_name[len] = '\0';

        size_t name_len = std::strlen(current_name) + 1;
        char* temp = (char*)realloc(ptr, count + name_len);
        if (temp == nullptr) {
            std::cout << "Memory allocation failed!" << std::endl;
            free(ptr);
            free(current_name);
            file.close();
            return -1;
        }
        ptr = temp;

        errno_t err = strcpy_s(ptr + count, name_len, current_name);
        if (err != 0) {
            std::cout << "Safe copy error!" << std::endl;
            free(ptr);
            free(current_name);
            file.close();
            return -1;
        }

        count += name_len;
    }

    file.close();
    for (size_t i = 0; i < count;) {
        std::cout << (ptr + i) << std::endl;
        i += std::strlen(ptr + i) + 1;
    }

    free(ptr);
    free(current_name);
    return 0;
}

int main(int argc, char* argv[]) {
    // Call the appropriate function
    names_function(argc, argv);
    return 0;
}
