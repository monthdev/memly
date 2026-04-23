#include "App/Bootstrap/DatabaseContext.hpp"

#include <duckdb.hpp>

#include <QtGlobal>

#include "Infrastructure/Sql/SchemaMigrator.hpp"

namespace App::Bootstrap {

DatabaseContext::DatabaseContext(const QString& DatabaseFilePath)
    : m_Database{}
    , m_DatabaseConnection{} {
    Q_ASSERT(not DatabaseFilePath.isEmpty());
    m_Database = std::make_unique<duckdb::DuckDB>(DatabaseFilePath.toStdString());
    m_DatabaseConnection = std::make_unique<duckdb::Connection>(*m_Database);
    Infrastructure::Sql::RunDatabaseBootstrap(*m_DatabaseConnection);
}

DatabaseContext::~DatabaseContext() = default;

duckdb::Connection& DatabaseContext::GetConnection() noexcept {
    Q_ASSERT(m_DatabaseConnection != nullptr);
    return *m_DatabaseConnection;
}

}
