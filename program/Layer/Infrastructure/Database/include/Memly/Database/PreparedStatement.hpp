#pragma once

#include <duckdb.hpp>

#include <memory>
#include <source_location>

#include "Memly/Database/PreparedStatementExecution.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

class [[nodiscard]] PreparedStatement final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::unique_ptr<duckdb::PreparedStatement> m_DuckDbPreparedStatement;

public:
    explicit PreparedStatement(std::unique_ptr<duckdb::PreparedStatement>&&) noexcept;

    [[nodiscard]] auto Execute(const std::source_location& = std::source_location::current()) noexcept -> PreparedStatementExecution;
};

}
