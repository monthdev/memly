#include "Memly/Database/TransactionRunner.hpp"

#include <duckdb.hpp>

namespace Layer::Infrastructure::Database {

TransactionRunner::TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
    : m_DatabaseConnection{ DatabaseConnection } {
}

}
