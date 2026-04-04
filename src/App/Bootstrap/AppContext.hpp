#pragma once

#include <QString>
#include <memory>

namespace duckdb {
class Connection;
class DuckDB;
}

namespace Store {
class DeckListStore;
}

namespace App {

class AppContext final {
public:
    static void Initialize(const QString& DatabaseFilePath);
    static Store::DeckListStore& GetRequiredDeckListStore() noexcept;

private:
    static std::unique_ptr<duckdb::DuckDB> s_Database;
    static std::unique_ptr<duckdb::Connection> s_DatabaseConnection;
    static std::unique_ptr<Store::DeckListStore> s_DeckListStore;
};

}
