module;

#include <duckdb.hpp>

#if not defined(NDEBUG)
#include <algorithm>
#include <ranges>
#endif

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

module Memly.Database.DatabaseMigrator;

import Memly.Database.DatabaseRuntime;
import Memly.Database.Internal.MigrationSql;
import Memly.Database.Internal.ThrowOnDatabaseError;
import Memly.Database.TransactionRunner;
import Memly.Exception.MemlyException;

namespace Memly::Database {

DatabaseMigrator::DatabaseMigrator(const std::string& DatabaseFilePath)
    : m_Database{ DatabaseFilePath }
    , m_DatabaseConnection{ this->m_Database } {
}

[[nodiscard]] auto DatabaseMigrator::ApplyMigrations() && -> DatabaseRuntime {
    TransactionRunner{ this->m_DatabaseConnection }.TransactionWrapper([this]() -> void {
        Internal::ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Internal::CreateMigrationsLogSql()));
        duckdb::unique_ptr<duckdb::MaterializedQueryResult> AppliedMigrationVersionsMaterializedQueryResult{ this->m_DatabaseConnection.Query(
            Internal::SelectAppliedMigrationVersionsSql()) };
        Internal::ThrowOnQueryResultError(*AppliedMigrationVersionsMaterializedQueryResult);
        std::vector<std::size_t> AppliedMigrationVersionVector{};
        // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
        for (const auto& QueryResultRow : *AppliedMigrationVersionsMaterializedQueryResult) {
            AppliedMigrationVersionVector.emplace_back(QueryResultRow.GetValue<std::uint32_t>(0));
        }
        const std::size_t AppliedMigrationCount{ AppliedMigrationVersionVector.size() };
        assert(std::ranges::equal(AppliedMigrationVersionVector, std::views::iota(std::size_t{ 1 }, AppliedMigrationCount + std::size_t{ 1 })));
        constexpr std::size_t MigrationSqlFunctionCount{ 2 };
        const std::array<std::string (*)(), MigrationSqlFunctionCount> MigrationSqlFunctionArray{ &Internal::M01CreateSchemaSql,
                                                                                                  &Internal::M02SeedTableDefaultsSql };
        const std::size_t AvailableMigrationCount{ MigrationSqlFunctionArray.size() };
        if (AppliedMigrationCount > AvailableMigrationCount) {
            throw Exception::MemlyException{ std::initializer_list<std::string_view>{ "Applied migration count exceeds available migration count" } };
        }
        if (AppliedMigrationCount < AvailableMigrationCount) {
            duckdb::unique_ptr<duckdb::PreparedStatement> InsertMigrationLogEntryPreparedStatement{ this->m_DatabaseConnection.Prepare(
                Internal::InsertMigrationLogEntrySql()) };
            Internal::ThrowOnPreparedStatementError(*InsertMigrationLogEntryPreparedStatement);
            for (std::size_t MigrationIndex{ AppliedMigrationCount }; MigrationIndex < AvailableMigrationCount; ++MigrationIndex) {
                Internal::ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(std::invoke(MigrationSqlFunctionArray.at(MigrationIndex))));
                Internal::ThrowOnQueryResultError(
                    *InsertMigrationLogEntryPreparedStatement->Execute(static_cast<std::uint32_t>(MigrationIndex + std::size_t{ 1 })));
            }
        }
    });
    return DatabaseRuntime{ *this->m_Database.instance, std::move(this->m_DatabaseConnection) };
}

}
