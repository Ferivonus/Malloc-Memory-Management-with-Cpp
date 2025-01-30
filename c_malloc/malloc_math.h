#ifndef MALLOC_MATH_H
#define MALLOC_MATH_H

#include <string>

class MallocMath {
private:
    int* numbers;
    size_t count;
    size_t totalSize;

    void allocateMemory(size_t newSize);

public:
    MallocMath();
    ~MallocMath();

    void loadNumbersFromFile(const std::string& filename);
    int performAddition() const;
    int performSubtraction() const;
    int performMultiplication() const;
    double performDivision() const;

    void printNumbers() const;
};

#endif // MALLOC_MATH_H
