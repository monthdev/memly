#include "Infrastructure/Database/DatabaseRuntime.hpp"

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

#include "Infrastructure/Database/PreparedStatement.hpp"
#include "Infrastructure/Database/PreparedStatementExecution.hpp"
#include "Infrastructure/Database/TransactionRunner.hpp"
#include "Infrastructure/Sql/Migration/MigrationSql.hpp"
#include "Infrastructure/Sql/Seed/SeedSql.hpp"
#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Infrastructure::Database {

[[nodiscard]] auto DatabaseRuntime::PrepareStatement(const std::string& Sql, const std::source_location& SourceLocation) -> PreparedStatement {
    std::unique_ptr<duckdb::PreparedStatement> DuckDbPreparedStatement{ m_DatabaseConnection.Prepare(Sql) };
    if (DuckDbPreparedStatement->HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ DuckDbPreparedStatement->GetError() }, SourceLocation);
    }
    return PreparedStatement{ std::move(DuckDbPreparedStatement) };
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
[[nodiscard]] auto DatabaseRuntime::ExecutePreparedStatement(PreparedStatement& PreparedStatement, const std::source_location& SourceLocation) noexcept
    -> PreparedStatementExecution {
    return PreparedStatementExecution{ *PreparedStatement.m_DuckDbPreparedStatement, SourceLocation };
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
[[nodiscard]] auto DatabaseRuntime::FetchNextDataChunk(duckdb::QueryResult& QueryResult, const std::source_location& SourceLocation)
    -> duckdb::unique_ptr<duckdb::DataChunk> {
    duckdb::unique_ptr<duckdb::DataChunk> DataChunk{};
    duckdb::ErrorData FetchError{};
    if (not QueryResult.TryFetch(DataChunk, FetchError)) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ FetchError.Message() }, SourceLocation);
    }
    return DataChunk;
}

[[nodiscard]] auto DatabaseRuntime::ExecuteSql(const std::string& Sql, const std::source_location& SourceLocation) -> std::unique_ptr<duckdb::QueryResult> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.SendQuery(Sql) };
    if (QueryResult->HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ QueryResult->GetError() }, SourceLocation);
    }
    return QueryResult;
}

[[nodiscard]] auto DatabaseRuntime::GetTransactionRunner() noexcept -> TransactionRunner& {
    return m_TransactionRunner;
}

void DatabaseRuntime::BootstrapDatabase() {
    m_TransactionRunner.TransactionWrapper([this]() -> void {
        ApplySchemaMigrations();
        SeedTableDefaults();
    });
}

void DatabaseRuntime::ApplySchemaMigrations() {
    static_cast<void>(ExecuteSql(Sql::Migration::M00_SchemaMigrationsLogSql()));
    std::unique_ptr<duckdb::QueryResult> QueryResult{ ExecuteSql(Sql::Migration::ReadSchemaMigrationsLogSql()) };
    std::vector<std::size_t> AppliedMigrationVersionSequenceVector{};
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ FetchNextDataChunk(*QueryResult) }) {
        for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size(); ++RowIndex) {
            AppliedMigrationVersionSequenceVector.push_back(DataChunk->GetValue(0, RowIndex).GetValue<std::uint32_t>());
        }
    }
    assert(std::ranges::equal(AppliedMigrationVersionSequenceVector,
                              std::views::iota(std::size_t{ 1 }, AppliedMigrationVersionSequenceVector.size() + std::size_t{ 1 })));
    const std::array<std::string (*)(), 1> SchemaMigrationSqlFunctionArray{ &Sql::Migration::M01_InitialSchemaSql };
    if (AppliedMigrationVersionSequenceVector.size() > SchemaMigrationSqlFunctionArray.size()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ "Unexpected number of applied migrations" });
    }
    if (AppliedMigrationVersionSequenceVector.size() < SchemaMigrationSqlFunctionArray.size()) {
        PreparedStatement CreateSchemaMigrationsLogEntryPreparedStatement{ PrepareStatement(Sql::Migration::CreateSchemaMigrationsLogEntrySql()) };
        for (std::size_t MigrationIndex{ AppliedMigrationVersionSequenceVector.size() }; MigrationIndex < SchemaMigrationSqlFunctionArray.size();
             ++MigrationIndex) {
            static_cast<void>(ExecuteSql(std::invoke(SchemaMigrationSqlFunctionArray.at(MigrationIndex))));
            static_cast<void>(
                ExecutePreparedStatement(CreateSchemaMigrationsLogEntryPreparedStatement).WithParameters(static_cast<std::uint32_t>(MigrationIndex + 1)));
        }
    }
}

void DatabaseRuntime::SeedTableDefaults() {
    static_cast<void>(ExecuteSql(Sql::Seed::CreateDefaultFsrs7SchedulerSql()));
    static_cast<void>(ExecuteSql(Sql::Seed::CreateDefaultFsrs7SettingsSql()));
    static_cast<void>(ExecuteSql(Sql::Seed::CreateDefaultDeckSettingsSql()));
}

}
