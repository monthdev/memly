#include "App/Bootstrap/AppContext.hpp"

#include <duckdb.hpp>

#include <QtGlobal>

#include "Sql/SchemaMigrator.hpp"
#include "Store/DeckListStore.hpp"

namespace App {

std::unique_ptr<duckdb::DuckDB> AppContext::s_Database{};
std::unique_ptr<duckdb::Connection> AppContext::s_DatabaseConnection{};
std::unique_ptr<Store::DeckListStore> AppContext::s_DeckListStore{};

void AppContext::Initialize(const QString& DatabaseFilePath) {
    Q_ASSERT(!DatabaseFilePath.isEmpty());
    Q_ASSERT(s_Database == nullptr);
    Q_ASSERT(s_DatabaseConnection == nullptr);
    Q_ASSERT(s_DeckListStore == nullptr);

    s_Database = std::make_unique<duckdb::DuckDB>(DatabaseFilePath.toStdString());
    s_DatabaseConnection = std::make_unique<duckdb::Connection>(*s_Database);
    Sql::RunDatabaseBootstrap(*s_DatabaseConnection);
    s_DeckListStore = std::make_unique<Store::DeckListStore>(*s_DatabaseConnection);
}

Store::DeckListStore& AppContext::GetRequiredDeckListStore() noexcept {
    Q_ASSERT(s_DeckListStore != nullptr);
    return *s_DeckListStore;
}

}
