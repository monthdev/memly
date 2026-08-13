#pragma once

#include <duckdb.hpp>

#include <source_location>
#include <utility>

#include "Memly/Database/QueryResultDecoder.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

class [[nodiscard]] PreparedStatementExecution final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::PreparedStatement& m_DuckDbPreparedStatement;
    const std::source_location& m_SourceLocation;

public:
    explicit PreparedStatementExecution(duckdb::PreparedStatement&, const std::source_location&) noexcept;

    template <typename... SqlParameterType>
        requires(sizeof...(SqlParameterType) > 0)
    [[nodiscard]] auto WithParameters(SqlParameterType&&... SqlParameters) && -> QueryResultDecoder {
        duckdb::vector<duckdb::Value> DuckDbValueVector{};
        DuckDbValueVector.reserve(sizeof...(SqlParameterType));
        (DuckDbValueVector.emplace_back(duckdb::Value::CreateValue(std::forward<SqlParameterType>(SqlParameters))), ...);
        return QueryResultDecoder{ this->Execute(std::move(DuckDbValueVector)) };
    }

    [[nodiscard]] auto WithoutParameters() && -> QueryResultDecoder;

private:
    [[nodiscard]] auto Execute(duckdb::vector<duckdb::Value>&&) -> QueryResultDecoder;
};

}
