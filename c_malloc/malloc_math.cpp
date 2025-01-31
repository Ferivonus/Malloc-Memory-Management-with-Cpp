#include "malloc_math.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iomanip>

const size_t MAX_MEMORY = 1024 * 1024 * 100;  // 100MB memory limit in here, if u try to break the app LOL.

MallocMath::MallocMath() : numbers(nullptr), count(0), totalSize(0) {}

MallocMath::~MallocMath() {
    free(numbers);
}

void MallocMath::allocateMemory(size_t newSize) {
    if (newSize > MAX_MEMORY) {
        std::cerr << "Memory usage exceeded the limit of " << MAX_MEMORY / (static_cast<unsigned long long>(1024) * 1024) << " MB!" << std::endl;
        free(numbers);
        numbers = nullptr;
        throw std::runtime_error("Exceeded memory limit");
    }

    int* temp = static_cast<int*>(realloc(numbers, newSize));
    if (!temp) {
        std::cerr << "Error allocating memory. Requested size: " << newSize << " bytes." << std::endl;
        free(numbers);
        numbers = nullptr;
        throw std::bad_alloc();
    }

    numbers = temp;
    totalSize = newSize;
}

void MallocMath::loadNumbersFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Error: Unable to open file '" + filename + "' for reading.");
    }

    // Free previous data and reset variables
    free(numbers);
    numbers = nullptr;
    count = 0;
    totalSize = 0;

    int num;
    while (file >> num) {
        size_t newSize = (count + 1) * sizeof(int);
        allocateMemory(newSize);

        // Ensure memory is properly allocated before using it
        if (numbers) {
            *(numbers + count) = num;  // Use pointer arithmetic
            count++;
        }
        else {
            throw std::runtime_error("Memory allocation failed while loading numbers.");
        }
    }

    file.close();
}

int MallocMath::performAddition() {
    int sum = 0;
    for (size_t i = 0; i < count; i++) {
        if (numbers) {
            sum += *(numbers + i);  // Use pointer arithmetic
        }
    }
    return sum;
}

int MallocMath::performSubtraction() {
    if (count == 0) throw std::runtime_error("No numbers available for subtraction.");

    int result = *numbers;  // Start with the first number
    for (size_t i = 1; i < count; i++) {
        if (numbers) {
            result -= *(numbers + i);  // Use pointer arithmetic
        }
    }
    return result;
}

int MallocMath::performMultiplication() {
    if (count == 0) throw std::runtime_error("No numbers available for multiplication.");

    int product = 1;
    for (size_t i = 0; i < count; i++) {
        if (numbers) {
            product *= *(numbers + i);  // Use pointer arithmetic
        }
    }
    return product;
}

double MallocMath::performDivision() {
    if (count == 0) throw std::runtime_error("No numbers available for division.");

    double result = static_cast<double>(*numbers);  // Start with the first number
    for (size_t i = 1; i < count; i++) {
        if (numbers) {
            if (*(numbers + i) == 0) {
                std::cerr << "Error: Division by zero encountered while dividing by " << *(numbers + i) << "." << std::endl;
                throw std::runtime_error("Division by zero encountered.");
            }
            result /= *(numbers + i);  // Use pointer arithmetic
        }
    }
    return result;
}

void MallocMath::printNumbers() const {
    if (count == 0) {
        std::cout << "No numbers stored!" << std::endl;
        return;
    }

    std::cout << "Stored numbers: ";
    for (size_t i = 0; i < count; i++) {
        if (numbers) {
            std::cout << *(numbers + i) << " ";  // Use pointer arithmetic
        }
    }
    std::cout << std::endl;
}

void MallocMath::performAllCalculationsAndWriteToFile() {
    std::ofstream file("results.txt");  // Writing directly to 'results.txt'
    if (!file) {
        std::cerr << "Error: Unable to open file 'results.txt' for writing." << std::endl;
        return;
    }

    // Print a header
    file << "Results of MallocMath Calculations\n";
    file << "=================================\n";
    file << std::fixed << std::setprecision(2);  // Set precision for floating point values

    try {
        // Perform addition
        int sum = performAddition();
        file << "Addition Result: " << sum << std::endl;

        // Perform subtraction
        int difference = performSubtraction();
        file << "Subtraction Result: " << difference << std::endl;

        // Perform multiplication
        int product = performMultiplication();
        file << "Multiplication Result: " << product << std::endl;

        // Perform division
        double quotient = performDivision();
        file << "Division Result: " << quotient << std::endl;

    }
    catch (const std::exception& e) {
        file << "Error during calculations: " << e.what() << std::endl;
    }

    // Print the numbers used in the calculations
    file << "\nStored Numbers: ";
    for (size_t i = 0; i < count; i++) {
        if (numbers) {
            file << *(numbers + i) << " ";  // Use pointer arithmetic
        }
    }
    file << std::endl;

    file.close();
    std::cout << "Calculations and results have been written to 'results.txt'." << std::endl;
}
