#pragma once

#include <duckdb.hpp>

#include <source_location>
#include <string>

#include "Layer/Infrastructure/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Database/TransactionRunner.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

class DatabaseRuntime final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;
    TransactionRunner m_TransactionRunner;

public:
    explicit DatabaseRuntime(const std::string& DatabaseFilePath)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_Database{ DatabaseFilePath }
        , m_DatabaseConnection{ this->m_Database }
        , m_TransactionRunner{ this->m_DatabaseConnection } {
        this->BootstrapDatabase();
    }

    [[nodiscard]] auto PrepareStatement(const std::string&, const std::source_location& = std::source_location::current()) -> PreparedStatement;

    [[nodiscard]] auto GetTransactionRunner() noexcept -> TransactionRunner&;

private:
    void BootstrapDatabase();
};

}
