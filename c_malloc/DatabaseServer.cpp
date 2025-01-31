#include "DatabaseServer.h"
#include "sqlite3.h"
#include <fstream>

using json = nlohmann::json;
using namespace httplib;

DatabaseServer::DatabaseServer(const std::string& dbName, int port)
    : port(port) {
    connectToDatabase(dbName);
    createTable(); // Create the table on initialization
    setupRoutes(); // Setup the routes inside the constructor
}

void DatabaseServer::connectToDatabase(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(1); // Exit if the database connection fails
    }
}

void DatabaseServer::createTable() {
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            phone_number TEXT NOT NULL,
            department TEXT NOT NULL
        );
    )";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Error creating table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void DatabaseServer::setupRoutes() {
    server.Get("/students", [this](const Request& req, Response& res) { handleGetStudents(req, res); });
    server.Post("/insert", [this](const Request& req, Response& res) { handleInsertStudent(req, res); });
    server.Post("/upload", [this](const Request& req, Response& res) { handleUploadFile(req, res); });
}

void DatabaseServer::handleGetStudents(const Request& req, Response& res) {
    sqlite3_stmt* stmt;
    std::string sql = "SELECT * FROM students";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        res.status = 500;
        res.set_content("Failed to retrieve data", "text/plain");
        return;
    }

    json students = json::array();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        json student = {
            {"id", sqlite3_column_int(stmt, 0)},
            {"first_name", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))},
            {"last_name", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))},
            {"phone_number", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))},
            {"department", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))}
        };
        students.push_back(student);
    }
    sqlite3_finalize(stmt);
    res.set_content(students.dump(), "application/json");
}

void DatabaseServer::handleInsertStudent(const Request& req, Response& res) {
    try {
        json body = json::parse(req.body);
        std::string firstName = body["first_name"];
        std::string lastName = body["last_name"];
        std::string phoneNumber = body["phone_number"];
        std::string department = body["department"];

        std::string sql = "INSERT INTO students (first_name, last_name, phone_number, department) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lastName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, phoneNumber.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, department.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            res.set_content("Student inserted successfully", "text/plain");
        }
        else {
            res.status = 500;
            res.set_content("Error inserting student", "text/plain");
        }
        sqlite3_finalize(stmt);
    }
    catch (const std::exception& e) {
        res.status = 400;
        res.set_content(std::string("Invalid request: ") + e.what(), "text/plain");
    }
}

void DatabaseServer::handleUploadFile(const Request& req, Response& res) {
    if (req.has_file("file")) {
        const auto& file = req.get_file_value("file");
        std::ofstream out(file.filename, std::ios::binary);
        if (!out) {
            res.status = 500;
            res.set_content("Failed to write file", "text/plain");
            return;
        }
        out.write(file.content.c_str(), file.content.size());
        out.close();

        // Pass the file path to insertFromTxtFile
        insertFromTxtFile(file.filename);
        res.set_content("File uploaded and processed successfully", "text/plain");
    }
    else {
        res.status = 400;
        res.set_content("No file uploaded", "text/plain");
    }
}

void DatabaseServer::insertFromTxtFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string firstName, lastName, phoneNumber, department;
    while (file >> firstName >> lastName >> phoneNumber >> department) {
        std::string sql = "INSERT INTO students (first_name, last_name, phone_number, department) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lastName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, phoneNumber.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, department.c_str(), -1, SQLITE_STATIC);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    file.close();
}

void DatabaseServer::start() {
    std::cout << "Server running on port " << port << "...\n";
    server.listen("0.0.0.0", port); // Start the server on the specified port
}
