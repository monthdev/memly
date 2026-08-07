#include "Layer/Infrastructure/DuckDb/Database/PreparedStatement.hpp"

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <utility>

#include "Layer/Infrastructure/DuckDb/Database/PreparedStatementExecution.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

PreparedStatement::PreparedStatement(std::unique_ptr<duckdb::PreparedStatement>&& DuckDbPreparedStatement) noexcept
    : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_DuckDbPreparedStatement{ std::move(DuckDbPreparedStatement) } {
}

[[nodiscard]] auto PreparedStatement::Execute(const std::source_location& SourceLocation) noexcept -> PreparedStatementExecution {
    return PreparedStatementExecution{ *this->m_DuckDbPreparedStatement, SourceLocation };
}

}
