#ifndef DATABASE_SERVER_H
#define DATABASE_SERVER_H

#include <string>
#include "httplib.h"
#include <sqlite3.h>
#include <nlohmann/json.hpp>


    class DatabaseServer {
    public:
        DatabaseServer(const std::string& dbName, int port);

        ~DatabaseServer();

        void run();

    private:
        // Private helper functions to interact with the database and handle requests
        void connectToDatabase(const std::string& dbName);
        void createTables();
        void setupRoutes();
        void executeSQL(const std::string& sql, const std::string& errorMessage);

        void printRoutes();

        // HTTP request handlers
        void handleGetBooks(const httplib::Request& req, httplib::Response& res);
        void handleGetBookById(const httplib::Request& req, httplib::Response& res);
        void handleInsertBook(const httplib::Request& req, httplib::Response& res);
        void handleUpdateBook(const httplib::Request& req, httplib::Response& res);
        void handleDeleteBook(const httplib::Request& req, httplib::Response& res);
        void handleUploadFile(const httplib::Request& req, httplib::Response& res);
        void handleError(httplib::Response& res, int status, const std::string& message);
        nlohmann::json extractBookData(sqlite3_stmt* stmt);

        sqlite3* db;

        httplib::Server server;

        int port;
    };


#endif // DATABASE_SERVER_H