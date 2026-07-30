#pragma once

#include <duckdb.hpp>

#include <memory>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Database {

class DatabaseRuntime;

class [[nodiscard]] PreparedStatement final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    friend class DatabaseRuntime;

private:
    std::unique_ptr<duckdb::PreparedStatement> m_DuckDbPreparedStatement;

    explicit PreparedStatement(std::unique_ptr<duckdb::PreparedStatement>&& DuckDbPreparedStatement) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DuckDbPreparedStatement{ std::move(DuckDbPreparedStatement) } {
    }
};

}
