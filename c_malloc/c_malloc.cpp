// c_malloc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cstdlib> // For malloc, realloc
#include <cstring> // For strcpy
#include <string>

// Function to calculate the sum of integers from a file
static int integer_sum(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename.txt>" << std::endl;
        return -1;
    }

    std::ifstream file(argv[1]); // Open the file
    if (!file) {
        std::cout << "File could not be opened!" << std::endl;
        return -1;
    }

    int* ptr = nullptr;
    int num, count = 0, sum = 0;

    while (file >> num) { // Read numbers from file
        int* temp = (int*)realloc(ptr, (static_cast<unsigned long long>(count) + 1) * sizeof(int));
        if (temp == nullptr) {
            std::cout << "Memory allocation failed!" << std::endl;
            free(ptr);
            file.close();
            return -1;
        }
        ptr = temp;
        ptr[count] = num; // Add number to array
        sum += num;
        count++;
    }

    file.close();

    std::cout << "Total: " << sum << std::endl;

    free(ptr); // Free memory

    return 0;
}

// Function to read names from a UTF-8 encoded file and concatenate them
static int names_function(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename.txt>" << std::endl;
        return -1;
    }

    std::ifstream file(argv[1], std::ios::in | std::ios::binary); // Open file in binary mode
    if (!file) {
        std::cout << "File could not be opened!" << std::endl;
        return -1;
    }

    char* ptr = nullptr; // Pointer to store concatenated names
    size_t count = 0;  // Counter for total length of names
    char* current_name = nullptr; // Pointer for each name

    std::string line;
    while (std::getline(file, line)) { // Read each line (name) from file
        std::wstring wide_line(line.begin(), line.end()); // Convert to wide string

        current_name = (char*)malloc(wide_line.size() + 1); // Allocate memory for the current name
        if (current_name == nullptr) {
            std::cout << "Memory allocation failed for current_name!" << std::endl;
            free(ptr); // Free previously allocated memory
            file.close();
            return -1;
        }

        size_t len = wide_line.size();
        for (size_t i = 0; i < len; ++i) {
            current_name[i] = static_cast<char>(wide_line[i]); // Copy wide string to char array
        }
        current_name[len] = '\0'; // Null-terminate the string

        size_t name_len = std::strlen(current_name) + 1;
        char* temp = (char*)realloc(ptr, count + name_len); // Reallocate memory for concatenation
        if (temp == nullptr) {
            std::cout << "Memory allocation failed!" << std::endl;
            free(ptr); // Free memory before exiting
            free(current_name); // Free current_name memory
            file.close();
            return -1;
        }
        ptr = temp;

        // Copy into the allocated memory
        errno_t err = strcpy_s(ptr + count, name_len, current_name);
        if (err != 0) {
            std::cout << "Safe copy error!" << std::endl;
            free(ptr); // Free memory before exiting
            free(current_name); // Free current_name memory
            file.close();
            return -1;
        }

        count += name_len; // Increment total length (including null terminator)
    }

    file.close();

    // Print the concatenated names
    for (size_t i = 0; i < count;) {
        std::cout << (ptr + i) << std::endl;
        i += std::strlen(ptr + i) + 1; // Move to the next name
    }

    free(ptr); // Free the memory for concatenated names
    free(current_name); // Free memory allocated for current_name

    return 0;
}

int main(int argc, char* argv[]) {
    // Call the function to sum integers or read names, as needed
    //integer_sum(argc, argv);
    names_function(argc, argv);

    return 0;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu