#ifndef DATABASESERVER_H
#define DATABASESERVER_H

#include "httplib.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include "sqlite3.h"

using json = nlohmann::json;
using namespace httplib;

class DatabaseServer {
public:
    DatabaseServer(const std::string& dbName, int port);  // Constructor that takes dbName and port
    void start(); // No need to pass port as argument here
    void setupRoutes();
    void handleGetStudents(const Request& req, Response& res);
    void handleInsertStudent(const Request& req, Response& res);
    void handleUploadFile(const Request& req, Response& res);
    void connectToDatabase(const std::string& dbName); // Helper function to connect to the database
    void createTable(); // Helper function to create the students table
    void insertFromTxtFile(const std::string& filename); // Helper function to insert data from a txt file

private:
    sqlite3* db;  // SQLite database connection
    Server server;
    int port;

    
};

#endif
