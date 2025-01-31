#include "DatabaseManager.h"
#include <iostream>
#include <fstream>

DatabaseManager::DatabaseManager(const std::string& dbName) : dbName(dbName), db(nullptr) {}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase() {
    if (sqlite3_open(dbName.c_str(), &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

void DatabaseManager::closeDatabase() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool DatabaseManager::createTable() {
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            phone_number TEXT NOT NULL,
            department TEXT NOT NULL
        );
    )";

    return executeSQL(createTableSQL);
}

bool DatabaseManager::insertStudent(const std::string& firstName, const std::string& lastName,
    const std::string& phoneNumber, const std::string& department) {
    std::string sql = "INSERT INTO students (first_name, last_name, phone_number, department) VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, lastName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, phoneNumber.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, department.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert student: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::getAllStudents() {
    std::string sql = "SELECT * FROM students";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* phoneNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        std::cout << "ID: " << id << ", Name: " << firstName << " " << lastName
            << ", Phone: " << phoneNumber << ", Department: " << department << std::endl;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::insertFromTxtFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string firstName, lastName, phoneNumber, department;

    while (file >> firstName >> lastName >> phoneNumber >> department) {
        if (!insertStudent(firstName, lastName, phoneNumber, department)) {
            std::cerr << "Failed to insert data from file." << std::endl;
            return false;
        }
    }

    file.close();
    return true;
}

bool DatabaseManager::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
