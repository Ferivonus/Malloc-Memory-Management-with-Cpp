#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include "sqlite3.h"

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbName);
    ~DatabaseManager();

    bool openDatabase();  // Opens the database connection
    void closeDatabase(); // Closes the database connection
    bool createTable();   // Creates the students table if it doesn't exist
    bool insertStudent(const std::string& firstName, const std::string& lastName,
        const std::string& phoneNumber, const std::string& department); // Insert a new student
    bool getAllStudents(); // Retrieves all students
    bool insertFromTxtFile(const std::string& filename); // Insert data from a txt file

private:
    sqlite3* db;
    std::string dbName;

    bool executeSQL(const std::string& sql); // Executes an SQL query
};

#endif
