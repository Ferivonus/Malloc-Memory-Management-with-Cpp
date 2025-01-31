#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbName);
    ~DatabaseManager();

    bool openDatabase();
    void closeDatabase();

    bool createTables();
    bool insertStudent(const std::string& firstName, const std::string& lastName,
        const std::string& phoneNumber, const std::string& department);
    bool getAllStudents();
    bool insertFromTxtFile(const std::string& filename);

    bool insertCourse(const std::string& courseName, const std::string& department, int credits);
    bool getAllCourses();
    bool enrollStudentInCourse(int studentId, int courseId);

    bool getStudentsInCourse(int courseId);
    bool getCoursesForStudent(int studentId);

private:
    bool executeSQL(const std::string& sql);
    bool createStudentTable();
    bool createCourseTable();
    bool createEnrollmentTable();

private:
    std::string dbName;
    sqlite3* db;
};

#endif
