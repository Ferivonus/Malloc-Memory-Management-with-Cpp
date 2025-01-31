#include "DatabaseServer.h"
#include "sqlite3.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "httplib.h"
#include <string>

using json = nlohmann::json;
using namespace httplib;

DatabaseServer::DatabaseServer(const std::string& dbName, int port)
    : port(port), db(nullptr) {
    connectToDatabase(dbName);
    createTables();  
    setupRoutes();   
}

DatabaseServer::~DatabaseServer() {
    if (db) {
        sqlite3_close(db);  
    }
}

void DatabaseServer::connectToDatabase(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);  
    }
}

void DatabaseServer::createTables() {
    const char* createBooksTableSQL = R"(
        CREATE TABLE IF NOT EXISTS books (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            author TEXT NOT NULL,
            isbn TEXT NOT NULL,
            year INTEGER NOT NULL,
            publisher TEXT NOT NULL,
            availability BOOLEAN NOT NULL
        );
    )";

    executeSQL(createBooksTableSQL, "Error creating books table");
}

void DatabaseServer::setupRoutes() {
    server.Get("/books", [this](const Request& req, Response& res) { handleGetBooks(req, res); });
    server.Get("/book/<id>", [this](const Request& req, Response& res) { handleGetBookById(req, res); });
    server.Post("/insert", [this](const Request& req, Response& res) { handleInsertBook(req, res); });
    server.Post("/upload", [this](const Request& req, Response& res) { handleUploadFile(req, res); });
    server.Put("/update/<id>", [this](const Request& req, Response& res) { handleUpdateBook(req, res); });
    server.Delete("/delete/<id>", [this](const Request& req, Response& res) { handleDeleteBook(req, res); });
}

void DatabaseServer::handleGetBooks(const Request& req, Response& res) {
    sqlite3_stmt* stmt = nullptr;
    std::string sql = "SELECT * FROM books";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        handleError(res, 500, "Failed to retrieve books");
        return;
    }

    json books = json::array();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        books.push_back(extractBookData(stmt));
    }

    sqlite3_finalize(stmt);
    res.set_content(books.dump(), "application/json");
}

void DatabaseServer::handleGetBookById(const Request& req, Response& res) {
    int id = std::stoi(req.matches[1]);
    sqlite3_stmt* stmt = nullptr;
    std::string sql = "SELECT * FROM books WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        handleError(res, 500, "Failed to retrieve book");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);
    json book;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        book = extractBookData(stmt);
    }

    sqlite3_finalize(stmt);

    if (book.empty()) {
        handleError(res, 404, "Book not found");
    }
    else {
        res.set_content(book.dump(), "application/json");
    }
}

void DatabaseServer::handleInsertBook(const Request& req, Response& res) {
    try {
        json body = json::parse(req.body);
        std::string title = body["title"];
        std::string author = body["author"];
        std::string isbn = body["isbn"];
        int year = body["year"];
        std::string publisher = body["publisher"];
        bool availability = body["availability"];

        std::string sql = "INSERT INTO books (title, author, isbn, year, publisher, availability) VALUES (?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare SQL statement");
        }

        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, author.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, isbn.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, year);
        sqlite3_bind_text(stmt, 5, publisher.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, availability ? 1 : 0);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            res.set_content("Book inserted successfully", "text/plain");
        }
        else {
            throw std::runtime_error("Error inserting book");
        }

        sqlite3_finalize(stmt);
    }
    catch (const std::exception& e) {
        handleError(res, 400, std::string("Invalid request: ") + e.what());
    }
}

void DatabaseServer::handleUpdateBook(const Request& req, Response& res) {
    int id = std::stoi(req.matches[1]);
    try {
        json body = json::parse(req.body);
        std::string title = body["title"];
        std::string author = body["author"];
        std::string isbn = body["isbn"];
        int year = body["year"];
        std::string publisher = body["publisher"];
        bool availability = body["availability"];

        std::string sql = "UPDATE books SET title = ?, author = ?, isbn = ?, year = ?, publisher = ?, availability = ? WHERE id = ?;";
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare SQL statement");
        }

        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, author.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, isbn.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, year);
        sqlite3_bind_text(stmt, 5, publisher.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, availability ? 1 : 0);
        sqlite3_bind_int(stmt, 7, id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            res.set_content("Book updated successfully", "text/plain");
        }
        else {
            throw std::runtime_error("Error updating book");
        }

        sqlite3_finalize(stmt);
    }
    catch (const std::exception& e) {
        handleError(res, 400, std::string("Invalid request: ") + e.what());
    }
}


void DatabaseServer::handleDeleteBook(const Request& req, Response& res) {
    int id = std::stoi(req.matches[1]);
    std::string sql = "DELETE FROM books WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        handleError(res, 500, "Failed to prepare delete statement");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        res.set_content("Book deleted successfully", "text/plain");
    }
    else {
        handleError(res, 500, "Error deleting book");
    }

    sqlite3_finalize(stmt);
}

void DatabaseServer::handleUploadFile(const Request& req, Response& res) {
    if (req.has_file("file")) {
        const auto& file = req.get_file_value("file");
        std::ofstream out(file.filename, std::ios::binary);

        if (!out) {
            handleError(res, 500, "Failed to write file");
            return;
        }

        out.write(file.content.data(), file.content.size());
        res.set_content("File uploaded successfully", "text/plain");
    }
    else {
        handleError(res, 400, "No file provided");
    }
}

void DatabaseServer::handleError(Response& res, int status, const std::string& message) {
    res.status = status;
    res.set_content(message, "text/plain");
}

json DatabaseServer::extractBookData(sqlite3_stmt* stmt) {
    json book;
    book["id"] = sqlite3_column_int(stmt, 0);
    book["title"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    book["author"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    book["isbn"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    book["year"] = sqlite3_column_int(stmt, 4);
    book["publisher"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    book["availability"] = sqlite3_column_int(stmt, 6) != 0;
    return book;
}

void DatabaseServer::executeSQL(const std::string& sql, const std::string& errorMessage) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << errorMessage << ": " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void DatabaseServer::run() {
    server.listen("localhost", port); 
}