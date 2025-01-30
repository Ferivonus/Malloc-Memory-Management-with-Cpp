#include "DatabaseManager.h"
#include <iostream>
#include <fstream>
#include <sstream>

DatabaseManager::DatabaseManager(const std::string& dbName) : db(nullptr), dbName(dbName) {
    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        throw std::runtime_error("Failed to open database.");
    }
}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

void DatabaseManager::createTable() {
    const char* sql = "CREATE TABLE IF NOT EXISTS students ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "first_name TEXT, "
        "last_name TEXT, "
        "phone_number TEXT, "
        "department TEXT);";

    char* errMessage = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMessage);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage);
    }
    else {
        std::cout << "Table created successfully.\n";
    }
}

void DatabaseManager::insertFromTxtFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string firstName, lastName, phoneNumber, department;

        std::getline(ss, firstName, ',');
        std::getline(ss, lastName, ',');
        std::getline(ss, phoneNumber, ',');
        std::getline(ss, department, ',');

        std::string sql = "INSERT INTO students (first_name, last_name, phone_number, department) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement\n";
            continue;
        }

        sqlite3_bind_text(stmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lastName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, phoneNumber.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, department.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to insert data\n";
        }
        else {
            std::cout << "Inserted: " << firstName << " " << lastName << " | " << phoneNumber << " | " << department << std::endl;
        }

        sqlite3_finalize(stmt);
    }
}
