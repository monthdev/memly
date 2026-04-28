#pragma once

#include <QString>
#include <memory>

namespace duckdb {
class Connection;
class DuckDB;
}

namespace Bootstrap {

class DatabaseContext final {
public:
    explicit DatabaseContext(const QString&);
    ~DatabaseContext();

    DatabaseContext(const DatabaseContext&) = delete;
    DatabaseContext& operator=(const DatabaseContext&) = delete;
    DatabaseContext(DatabaseContext&&) = delete;
    DatabaseContext& operator=(DatabaseContext&&) = delete;

    [[nodiscard]] duckdb::Connection& GetConnection() noexcept;

private:
    std::unique_ptr<duckdb::DuckDB> m_Database;
    std::unique_ptr<duckdb::Connection> m_DatabaseConnection;
};

}
