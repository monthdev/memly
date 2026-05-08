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

[[nodiscard]] const std::string& M00_MigrationsTableSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Migration/M00_MigrationsTable.sql") };
    return Sql;
}

[[nodiscard]] const std::string& M01_InitialSchemaSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Migration/M01_InitialSchema.sql") };
    return Sql;
}

[[nodiscard]] const std::string& CreateMigrationSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Migration/CreateMigration.sql") };
    return Sql;
}

[[nodiscard]] const std::string& ReadMigrationsTableSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Migration/ReadMigrationsTable.sql") };
    return Sql;
}

[[nodiscard]] const std::string& CreateDefaultFsrs7SchedulerSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultFsrs7Scheduler.sql") };
    return Sql;
}

[[nodiscard]] const std::string& CreateDefaultFsrs7SettingsSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultFsrs7Settings.sql") };
    return Sql;
}

[[nodiscard]] const std::string& CreateDefaultDeckSettingsSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultDeckSettings.sql") };
    return Sql;
}

[[nodiscard]] const std::string& CreateRootDeckSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Mutation/CreateRootDeck.sql") };
    return Sql;
}

[[nodiscard]] const std::string& CreateChildDeckSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Mutation/CreateChildDeck.sql") };
    return Sql;
}

[[nodiscard]] const std::string& ReadDeckTreeSnapshotSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Query/ReadDeckTreeSnapshot.sql") };
    return Sql;
}

[[nodiscard]] const std::string& ReadNextLibraryRefreshAtMillisecondsSinceEpochSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Query/ReadNextLibraryRefreshAtMillisecondsSinceEpoch.sql") };
    return Sql;
}

[[nodiscard]] const std::string& ValidateDeckExistsSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Query/ValidateDeckExists.sql") };
    return Sql;
}

[[nodiscard]] const std::string& ValidateParentDeckExistsSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Query/ValidateParentDeckExists.sql") };
    return Sql;
}

[[nodiscard]] const std::string& MoveDeckSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Mutation/MoveDeck.sql") };
    return Sql;
}

[[nodiscard]] const std::string& RenameDeckSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Mutation/RenameDeck.sql") };
    return Sql;
}

[[nodiscard]] const std::string& DeleteDeckCardReviewsSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeckCardReviews.sql") };
    return Sql;
}

[[nodiscard]] const std::string& DeleteDeckCardsSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeckCards.sql") };
    return Sql;
}

[[nodiscard]] const std::string& DeleteDeckSql() {
    static const std::string Sql{ ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeck.sql") };
    return Sql;
}
}
