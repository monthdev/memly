module;

#include <duckdb.hpp>

module Memly.Database;

namespace Memly::Database {

TransactionRunner::TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
    : m_DatabaseConnection{ DatabaseConnection } {
}

}
