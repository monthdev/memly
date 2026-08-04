#pragma once

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <utility>

#include "Layer/Infrastructure/Persistence/Database/PreparedStatementExecution.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Persistence::Database {

class DatabaseRuntime;

class [[nodiscard]] PreparedStatement final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    friend class DatabaseRuntime;

private:
    std::unique_ptr<duckdb::PreparedStatement> m_DuckDbPreparedStatement;

    explicit PreparedStatement(std::unique_ptr<duckdb::PreparedStatement>&& DuckDbPreparedStatement) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DuckDbPreparedStatement{ std::move(DuckDbPreparedStatement) } {
    }

public:
    [[nodiscard]] auto Execute(const std::source_location& = std::source_location::current()) noexcept -> PreparedStatementExecution;
};

}
