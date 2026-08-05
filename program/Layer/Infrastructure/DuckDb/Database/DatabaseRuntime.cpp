#include "Layer/Infrastructure/DuckDb/Database/DatabaseRuntime.hpp"

#include <duckdb.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <ranges>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Layer/Infrastructure/DuckDb/Database/DecodableQueryResultRowMixin.hpp"
#include "Layer/Infrastructure/DuckDb/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/DuckDb/Database/PreparedStatementExecution.hpp"
#include "Layer/Infrastructure/DuckDb/Database/QueryResultDecoder.hpp"
#include "Layer/Infrastructure/DuckDb/Database/QueryResultRowCountRange.hpp"
#include "Layer/Infrastructure/DuckDb/Database/Sql/Migration/MigrationSql.hpp"
#include "Layer/Infrastructure/DuckDb/Database/Sql/Seed/SeedSql.hpp"
#include "Layer/Infrastructure/DuckDb/Database/TransactionRunner.hpp"
#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

[[nodiscard]] auto DatabaseRuntime::PrepareStatement(const std::string& Sql, const std::source_location& SourceLocation) -> PreparedStatement {
    std::unique_ptr<duckdb::PreparedStatement> DuckDbPreparedStatement{ this->m_DatabaseConnection.Prepare(Sql) };
    if (DuckDbPreparedStatement->HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ DuckDbPreparedStatement->GetError() }, SourceLocation);
    }
    return PreparedStatement{ std::move(DuckDbPreparedStatement) };
}

[[nodiscard]] auto DatabaseRuntime::Query(const std::string& Sql, const std::source_location& SourceLocation) -> QueryResultDecoder {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ this->m_DatabaseConnection.SendQuery(Sql) };
    if (QueryResult->HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ QueryResult->GetError() }, SourceLocation);
    }
    return QueryResultDecoder{ std::move(QueryResult), SourceLocation };
}

[[nodiscard]] auto DatabaseRuntime::GetTransactionRunner() noexcept -> TransactionRunner& {
    return this->m_TransactionRunner;
}

void DatabaseRuntime::BootstrapDatabase() {
    this->m_TransactionRunner.TransactionWrapper([this]() -> void {
        this->ApplyMigrations();
        this->SeedTableDefaults();
    });
}

namespace {

struct [[nodiscard]] u_MigrationLogEntryRecord final : public DecodableQueryResultRowMixin<std::uint32_t> {
    std::size_t m_Version;

    [[maybe_unused]] explicit u_MigrationLogEntryRecord(const std::uint32_t Version)
        : DecodableQueryResultRowMixin{}
        , m_Version{ Version } {
    }
};

}

void DatabaseRuntime::ApplyMigrations() {
    static_cast<void>(this->Query(Sql::Migration::CreateMigrationsLogSql()));
    std::vector<u_MigrationLogEntryRecord> AppliedMigrationLogEntryRecordVector{ this->Query(Sql::Migration::SelectAppliedMigrationVersionsSql())
                                                                                     .DecodedTo<u_MigrationLogEntryRecord>()
                                                                                     .AssertRowCount(QueryResultRowCountRange::ZeroOrMore()) };
    const std::size_t AppliedMigrationCount{ AppliedMigrationLogEntryRecordVector.size() };
    assert(std::ranges::equal(AppliedMigrationLogEntryRecordVector,
                              std::views::iota(std::size_t{ 1 }, AppliedMigrationCount + std::size_t{ 1 }),
                              std::ranges::equal_to{},
                              &u_MigrationLogEntryRecord::m_Version));
    const std::array<std::string (*)(), 1> MigrationSqlFunctionArray{ &Sql::Migration::M01_CreateSchemaSql };
    const std::size_t AvailableMigrationCount{ MigrationSqlFunctionArray.size() };
    if (AppliedMigrationCount > AvailableMigrationCount) {
        Support::Runtime::Exception::ThrowMemlyException(
            std::initializer_list<std::string_view>{ "Applied migration count exceeds available migration count" });
    }
    if (AppliedMigrationCount < AvailableMigrationCount) {
        PreparedStatement InsertMigrationLogEntryPreparedStatement{ this->PrepareStatement(Sql::Migration::InsertMigrationLogEntrySql()) };
        for (std::size_t MigrationIndex{ AppliedMigrationCount }; MigrationIndex < AvailableMigrationCount; ++MigrationIndex) {
            static_cast<void>(this->Query(std::invoke(MigrationSqlFunctionArray.at(MigrationIndex))));
            static_cast<void>(InsertMigrationLogEntryPreparedStatement.Execute().WithParameters(static_cast<std::uint32_t>(MigrationIndex + std::size_t{ 1 })));
        }
    }
}

void DatabaseRuntime::SeedTableDefaults() {
    static_cast<void>(this->Query(Sql::Seed::InsertDefaultFsrs7SchedulerSql()));
    static_cast<void>(this->Query(Sql::Seed::InsertDefaultFsrs7SettingsSql()));
    static_cast<void>(this->Query(Sql::Seed::InsertDefaultDeckSettingsSql()));
}

}
