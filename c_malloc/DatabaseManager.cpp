#include "DatabaseManager.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>

// Constructor and Destructor
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

// Create all tables (students, courses, enrollments)
bool DatabaseManager::createTables() {
    return createStudentTable() && createCourseTable() && createEnrollmentTable();
}

bool DatabaseManager::createStudentTable() {
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

bool DatabaseManager::createCourseTable() {
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS courses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            course_name TEXT NOT NULL,
            department TEXT NOT NULL,
            credits INTEGER NOT NULL
        );
    )";
    return executeSQL(createTableSQL);
}

// Create enrollment table (many-to-many relation)
bool DatabaseManager::createEnrollmentTable() {
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS enrollments (
            student_id INTEGER NOT NULL,
            course_id INTEGER NOT NULL,
            PRIMARY KEY (student_id, course_id),
            FOREIGN KEY (student_id) REFERENCES students(id),
            FOREIGN KEY (course_id) REFERENCES courses(id)
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

bool DatabaseManager::insertCourse(const std::string& courseName, const std::string& department, int credits) {
    std::string sql = "INSERT INTO courses (course_name, department, credits) VALUES (?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, courseName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, department.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, credits);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert course: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::getAllCourses() {
    std::string sql = "SELECT * FROM courses";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* courseName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int credits = sqlite3_column_int(stmt, 3);

        std::cout << "ID: " << id << ", Course: " << courseName
            << ", Department: " << department << ", Credits: " << credits << std::endl;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::enrollStudentInCourse(int studentId, int courseId) {
    std::string sql = "INSERT INTO enrollments (student_id, course_id) VALUES (?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, studentId);
    sqlite3_bind_int(stmt, 2, courseId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to enroll student: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::getStudentsInCourse(int courseId) {
    std::string sql = "SELECT students.first_name, students.last_name FROM students "
        "JOIN enrollments ON students.id = enrollments.student_id "
        "WHERE enrollments.course_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, courseId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        std::cout << "Student: " << firstName << " " << lastName << std::endl;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::getCoursesForStudent(int studentId) {
    std::string sql = "SELECT courses.course_name FROM courses "
        "JOIN enrollments ON courses.id = enrollments.course_id "
        "WHERE enrollments.student_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, studentId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* courseName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        std::cout << "Course: " << courseName << std::endl;
    }

    sqlite3_finalize(stmt);
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
