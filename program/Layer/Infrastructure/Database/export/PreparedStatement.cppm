module;

#include <duckdb.hpp>

#include <memory>
#include <source_location>

export module Memly.Database.PreparedStatement;

import Memly.Database.PreparedStatementExecution;

export namespace Memly::Database {

class [[nodiscard]] PreparedStatement final {
private:
    std::unique_ptr<duckdb::PreparedStatement> m_DuckDbPreparedStatement;

public:
    explicit PreparedStatement(std::unique_ptr<duckdb::PreparedStatement>&&) noexcept;

    explicit PreparedStatement(const PreparedStatement&) = delete;
    auto operator=(const PreparedStatement&) -> PreparedStatement& = delete;

    explicit PreparedStatement(PreparedStatement&&) = delete;
    auto operator=(PreparedStatement&&) -> PreparedStatement& = delete;

    ~PreparedStatement() noexcept = default;

    [[nodiscard]] auto Execute(std::source_location = std::source_location::current()) noexcept -> PreparedStatementExecution;
};

}
