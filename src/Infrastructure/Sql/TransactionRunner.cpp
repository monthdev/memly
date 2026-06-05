#include "Infrastructure/Sql/TransactionRunner.hpp"

namespace Infrastructure::Sql {

TransactionRunner::TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
    : m_DatabaseConnection{ DatabaseConnection } {
}

}
