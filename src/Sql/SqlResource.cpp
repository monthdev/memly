#include "SqlResource.hpp"

#include <QResource>

#include "Support/Fatal.hpp"

namespace Sql {

static std::string ReadResourceBytes(const char* ResourcePath) {
    QResource Resource{ ResourcePath };
    if (!Resource.isValid()) {
        Support::ThrowError("Error opening SQL file");
    }
    return std::string{ reinterpret_cast<const char*>(Resource.data()),
                        static_cast<size_t>(Resource.size()) };
}

[[nodiscard]] std::string M00_MigrationsTableSql() {
    return ReadResourceBytes(":/Sql/Migration/M00_MigrationsTable.sql");
}

[[nodiscard]] std::string M01_InitialSchemaSql() {
    return ReadResourceBytes(":/Sql/Migration/M01_InitialSchema.sql");
}

[[nodiscard]] std::string CreateMigrationSql() {
    return ReadResourceBytes(":/Sql/MigrationStatement/CreateMigration.sql");
}

[[nodiscard]] std::string ReadMigrationsTableSql() {
    return ReadResourceBytes(
        ":/Sql/MigrationStatement/ReadMigrationsTable.sql");
}

[[nodiscard]] std::string CreateDeckSql() {
    return ReadResourceBytes(":/Sql/Statement/CreateDeck.sql");
}

[[nodiscard]] std::string ReadDecksTableSql() {
    return ReadResourceBytes(":/Sql/Statement/ReadDecksTable.sql");
}

[[nodiscard]] std::string UpdateDeckSql() {
    return ReadResourceBytes(":/Sql/Statement/UpdateDeck.sql");
}

[[nodiscard]] std::string DeleteDeckSql() {
    return ReadResourceBytes(":/Sql/Statement/DeleteDeck.sql");
}

}
