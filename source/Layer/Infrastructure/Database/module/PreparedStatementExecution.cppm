/// \file
/// \brief Captures the caller's location so later parameter binding does not require callers to pass `std::source_location` manually.

module;

#include <duckdb.hpp>

#include <source_location>
#include <utility>

export module Memly.Database.PreparedStatementExecution;

import Memly.Database.QueryResultDecoder;

export namespace Memly::Database {

class [[nodiscard]] PreparedStatementExecution final {
private:
    duckdb::PreparedStatement& m_DuckDbPreparedStatement;
    std::source_location m_SourceLocation;

public:
    explicit PreparedStatementExecution([[clang::lifetimebound]] duckdb::PreparedStatement&, std::source_location) noexcept;

    explicit PreparedStatementExecution(const PreparedStatementExecution&) = delete;
    auto operator=(const PreparedStatementExecution&) -> PreparedStatementExecution& = delete;

    explicit PreparedStatementExecution(PreparedStatementExecution&&) = delete;
    auto operator=(PreparedStatementExecution&&) -> PreparedStatementExecution& = delete;

    ~PreparedStatementExecution() noexcept = default;

    template <typename... SqlParameterType>
        requires(sizeof...(SqlParameterType) > 0)
    [[nodiscard]] auto WithParameters(SqlParameterType&&... SqlParameters) && -> QueryResultDecoder {
        duckdb::vector<duckdb::Value> DuckDbValueVector{};
        DuckDbValueVector.reserve(sizeof...(SqlParameterType));
        (DuckDbValueVector.emplace_back(duckdb::Value::CreateValue(std::forward<SqlParameterType>(SqlParameters))), ...);
        return QueryResultDecoder{ std::move(*this).Execute(std::move(DuckDbValueVector)) };
    }

    [[nodiscard]] auto WithoutParameters() && -> QueryResultDecoder;

private:
    [[nodiscard]] auto Execute(duckdb::vector<duckdb::Value>&&) && -> QueryResultDecoder;
};

}
