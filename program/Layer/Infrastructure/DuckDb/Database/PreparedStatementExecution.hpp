#pragma once

#include <duckdb.hpp>

#include <source_location>
#include <utility>

#include "Layer/Infrastructure/DuckDb/Database/QueryResultDecoder.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

class PreparedStatement;

class [[nodiscard]] PreparedStatementExecution final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    friend class PreparedStatement;

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
