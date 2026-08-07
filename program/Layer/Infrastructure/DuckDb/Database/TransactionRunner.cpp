#include "Layer/Infrastructure/DuckDb/Database/TransactionRunner.hpp"

#include <duckdb.hpp>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

TransactionRunner::TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
    : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_DatabaseConnection{ DatabaseConnection } {
}

}
