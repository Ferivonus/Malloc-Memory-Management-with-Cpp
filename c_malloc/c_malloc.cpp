#include "DatabaseManager.h"
#include "NameManager.h"
#include "malloc_math.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <mode> <filename.txt>\n";
        std::cerr << "Modes: math / names / db\n";
        return EXIT_FAILURE;
    }

    std::string mode = argv[1];
    std::string filename = argv[2];

    try {
        if (mode == "math") {
            MallocMath math;
            math.loadNumbersFromFile(filename);
            math.printNumbers();
            std::cout << "Addition: " << math.performAddition() << std::endl;
            std::cout << "Subtraction: " << math.performSubtraction() << std::endl;
            std::cout << "Multiplication: " << math.performMultiplication() << std::endl;
            std::cout << "Division: " << math.performDivision() << std::endl;
            math.performAllCalculationsAndWriteToFile();
        }
        else if (mode == "names") {
            NameManager nameManager;
            nameManager.readNamesFromFile(filename);
            nameManager.printNames();
        }
        else if (mode == "db") {
            DatabaseManager dbManager("students.db");
            dbManager.createTable();
            dbManager.insertFromTxtFile(filename);
        }
        else {
            std::cerr << "Invalid mode! Use 'math', 'names', or 'db'.\n";
            return EXIT_FAILURE;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
