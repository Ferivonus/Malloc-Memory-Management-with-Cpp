#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>
#include <vector>

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbName);
    ~DatabaseManager();

    void createTable();
    void insertFromTxtFile(const std::string& filename);

private:
    sqlite3* db;
    std::string dbName;
};

#endif
