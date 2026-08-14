#include "Memly/Database/TransactionRunner.hpp"

#include <duckdb.hpp>

namespace Memly::Database {

TransactionRunner::TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
    : m_DatabaseConnection{ DatabaseConnection } {
}

}
