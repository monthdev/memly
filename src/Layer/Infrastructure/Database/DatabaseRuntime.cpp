#include "Layer/Infrastructure/Database/DatabaseRuntime.hpp"

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

#include "Layer/Infrastructure/Database/DecodableQueryResultRowMixin.hpp"
#include "Layer/Infrastructure/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Database/PreparedStatementExecution.hpp"
#include "Layer/Infrastructure/Database/QueryResultDecoder.hpp"
#include "Layer/Infrastructure/Database/QueryResultRowCountRange.hpp"
#include "Layer/Infrastructure/Database/TransactionRunner.hpp"
#include "Layer/Infrastructure/Sql/Migration/MigrationSql.hpp"
#include "Layer/Infrastructure/Sql/Seed/SeedSql.hpp"
#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Infrastructure::Database {

[[nodiscard]] auto DatabaseRuntime::PrepareStatement(const std::string& Sql, const std::source_location& SourceLocation) -> PreparedStatement {
    std::unique_ptr<duckdb::PreparedStatement> DuckDbPreparedStatement{ this->m_DatabaseConnection.Prepare(Sql) };
    if (DuckDbPreparedStatement->HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ DuckDbPreparedStatement->GetError() }, SourceLocation);
    }
    return PreparedStatement{ std::move(DuckDbPreparedStatement) };
}

[[nodiscard]] auto DatabaseRuntime::ExecuteSql(const std::string& Sql, const std::source_location& SourceLocation) -> QueryResultDecoder {
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

struct [[nodiscard]] a_MigrationLogEntryRecord final : public DecodableQueryResultRowMixin<std::uint32_t> {
    std::size_t m_Version;

    [[maybe_unused]] explicit a_MigrationLogEntryRecord(const std::uint32_t Version)
        : DecodableQueryResultRowMixin{}
        , m_Version{ Version } {
    }
};

}

void DatabaseRuntime::ApplyMigrations() {
    static_cast<void>(this->ExecuteSql(Sql::Migration::M00_CreateMigrationsLogSql()));
    std::vector<a_MigrationLogEntryRecord> AppliedMigrationLogEntryRecordVector{
        this->ExecuteSql(Sql::Migration::ReadMigrationsLogSql()).DecodedTo<a_MigrationLogEntryRecord>().AssertRowCount(QueryResultRowCountRange::ZeroOrMore())
    };
    assert(std::ranges::equal(AppliedMigrationLogEntryRecordVector,
                              std::views::iota(std::size_t{ 1 }, AppliedMigrationLogEntryRecordVector.size() + std::size_t{ 1 }),
                              std::ranges::equal_to{},
                              &a_MigrationLogEntryRecord::m_Version));
    const std::array<std::string (*)(), 1> MigrationSqlFunctionArray{ &Sql::Migration::M01_CreateSchemaSql };
    if (AppliedMigrationLogEntryRecordVector.size() > MigrationSqlFunctionArray.size()) {
        Support::Runtime::Exception::ThrowMemlyException(
            std::initializer_list<std::string_view>{ "Applied migration count exceeds available migration count" });
    }
    if (AppliedMigrationLogEntryRecordVector.size() < MigrationSqlFunctionArray.size()) {
        PreparedStatement CreateMigrationsLogEntryPreparedStatement{ this->PrepareStatement(Sql::Migration::CreateMigrationsLogEntrySql()) };
        for (std::size_t MigrationIndex{ AppliedMigrationLogEntryRecordVector.size() }; MigrationIndex < MigrationSqlFunctionArray.size(); ++MigrationIndex) {
            static_cast<void>(this->ExecuteSql(std::invoke(MigrationSqlFunctionArray.at(MigrationIndex))));
            static_cast<void>(CreateMigrationsLogEntryPreparedStatement.Execute().WithParameters(static_cast<std::uint32_t>(MigrationIndex + 1)));
        }
    }
}

void DatabaseRuntime::SeedTableDefaults() {
    static_cast<void>(this->ExecuteSql(Sql::Seed::CreateDefaultFsrs7SchedulerSql()));
    static_cast<void>(this->ExecuteSql(Sql::Seed::CreateDefaultFsrs7SettingsSql()));
    static_cast<void>(this->ExecuteSql(Sql::Seed::CreateDefaultDeckSettingsSql()));
}

}
