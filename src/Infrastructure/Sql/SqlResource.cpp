#include "SqlResource.hpp"

#include <QResource>

#include "Support/Fatal.hpp"

namespace Infrastructure::Sql {

namespace {
std::string ReadSqlResourceBytes(const char* SqlResourcePath) {
    QResource SqlResource{ SqlResourcePath };
    if (not SqlResource.isValid()) {
        Support::ThrowError("Error opening SQL file");
    }
    const QByteArray SqlResourceBytes{ SqlResource.uncompressedData() };
    if (SqlResourceBytes.isNull()) {
        Support::ThrowError("Error reading SQL resource bytes");
    }
    return std::string{ SqlResourceBytes.constData(), static_cast<std::size_t>(SqlResourceBytes.size()) };
}
}

[[nodiscard]] std::string M00_MigrationsTableSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/M00_MigrationsTable.sql");
}

[[nodiscard]] std::string M01_InitialSchemaSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/M01_InitialSchema.sql");
}

[[nodiscard]] std::string CreateMigrationSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/CreateMigration.sql");
}

[[nodiscard]] std::string ReadMigrationsTableSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/ReadMigrationsTable.sql");
}

[[nodiscard]] std::string CreateDefaultFsrs7SchedulerSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultFsrs7Scheduler.sql");
}

[[nodiscard]] std::string CreateDefaultFsrs7SettingsSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultFsrs7Settings.sql");
}

[[nodiscard]] std::string CreateDefaultDeckSettingsSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultDeckSettings.sql");
}

[[nodiscard]] std::string CreateRootDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateRootDeck.sql");
}

[[nodiscard]] std::string CreateChildDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateChildDeck.sql");
}

[[nodiscard]] std::string ReadDeckTreeSnapshotSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReadDeckTreeSnapshot.sql");
}

[[nodiscard]] std::string ReadNextLibraryRefreshAtMillisecondsSinceEpochSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReadNextLibraryRefreshAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] std::string MoveDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/MoveDeck.sql");
}

[[nodiscard]] std::string UpdateDeckNameSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/UpdateDeckName.sql");
}

[[nodiscard]] std::string DeleteDeckCardReviewsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeckCardReviews.sql");
}

[[nodiscard]] std::string DeleteDeckCardsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeckCards.sql");
}

[[nodiscard]] std::string DeleteDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeck.sql");
}

}
