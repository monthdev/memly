#pragma once

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Database {

class DatabaseRuntime;

class [[nodiscard]] PreparedStatementExecution final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    friend class DatabaseRuntime;

private:
    duckdb::PreparedStatement& m_DuckDbPreparedStatement;
    const std::source_location& m_SourceLocation;

    explicit PreparedStatementExecution(duckdb::PreparedStatement& DuckDbPreparedStatement, const std::source_location& SourceLocation) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DuckDbPreparedStatement{ DuckDbPreparedStatement }
        , m_SourceLocation{ SourceLocation } {
    }

public:
    template <typename... SqlParameterType>
        requires(sizeof...(SqlParameterType) > 0)
    [[nodiscard]] auto WithParameters(SqlParameterType&&... SqlParameters) && -> std::unique_ptr<duckdb::QueryResult> {
        duckdb::vector<duckdb::Value> DuckDbValueVector{};
        DuckDbValueVector.reserve(sizeof...(SqlParameterType));
        (DuckDbValueVector.emplace_back(duckdb::Value::CreateValue(std::forward<SqlParameterType>(SqlParameters))), ...);
        return Execute(DuckDbValueVector);
    }

    [[nodiscard]] auto WithoutParameters() && -> std::unique_ptr<duckdb::QueryResult>;

private:
    [[nodiscard]] auto Execute(duckdb::vector<duckdb::Value>&) -> std::unique_ptr<duckdb::QueryResult>;
};

}
