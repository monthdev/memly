#include "Layer/Infrastructure/DuckDb/Database/PreparedStatement.hpp"

#include <source_location>

#include "Layer/Infrastructure/DuckDb/Database/PreparedStatementExecution.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

[[nodiscard]] auto PreparedStatement::Execute(const std::source_location& SourceLocation) noexcept -> PreparedStatementExecution {
    return PreparedStatementExecution{ *this->m_DuckDbPreparedStatement, SourceLocation };
}

}
