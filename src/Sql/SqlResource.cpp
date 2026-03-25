#include "SqlResource.hpp"

#include <QResource>

#include "Support/Fatal.hpp"

namespace Sql {

namespace {
std::string ReadSqlResourceBytes(const char* SqlResourcePath) {
    QResource SqlResource{ SqlResourcePath };
    if (!SqlResource.isValid()) {
        Support::ThrowError("Error opening SQL file");
    }
    const QByteArray SqlResourceBytes{ SqlResource.uncompressedData() };
    if (SqlResourceBytes.isNull()) {
        Support::ThrowError("Error reading SQL resource bytes");
    }
    return std::string{ SqlResourceBytes.constData(),
                        static_cast<std::size_t>(SqlResourceBytes.size()) };
}
}

[[nodiscard]] std::string M00_MigrationsTableSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/M00_MigrationsTable.sql");
}

[[nodiscard]] std::string M01_InitialSchemaSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/M01_InitialSchema.sql");
}

[[nodiscard]] std::string CreateMigrationSql() {
    return ReadSqlResourceBytes(":/Sql/MigrationStatement/CreateMigration.sql");
}

[[nodiscard]] std::string ReadMigrationsTableSql() {
    return ReadSqlResourceBytes(":/Sql/MigrationStatement/ReadMigrationsTable.sql");
}

[[nodiscard]] std::string CreateDefaultFsrsConfigurationSql() {
    return ReadSqlResourceBytes(":/Sql/SeedStatement/CreateDefaultFsrsConfiguration.sql");
}

[[nodiscard]] std::string CreateDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Statement/CreateDeck.sql");
}

[[nodiscard]] std::string ReadDecksTableSql() {
    return ReadSqlResourceBytes(":/Sql/Statement/ReadDecksTable.sql");
}

[[nodiscard]] std::string UpdateDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Statement/UpdateDeck.sql");
}

[[nodiscard]] std::string DeleteDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Statement/DeleteDeck.sql");
}

}
