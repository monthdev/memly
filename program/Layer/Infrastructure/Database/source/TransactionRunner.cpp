#include "Memly/Database/TransactionRunner.hpp"

#include <duckdb.hpp>

#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

TransactionRunner::TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
    : NoCopyNoMoveMixin{}
    , m_DatabaseConnection{ DatabaseConnection } {
}

}
