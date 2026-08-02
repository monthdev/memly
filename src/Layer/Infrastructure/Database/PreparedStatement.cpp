#include "Layer/Infrastructure/Database/PreparedStatement.hpp"

#include <source_location>

#include "Layer/Infrastructure/Database/PreparedStatementExecution.hpp"

namespace Layer::Infrastructure::Database {

[[nodiscard]] auto PreparedStatement::Execute(const std::source_location& SourceLocation) noexcept -> PreparedStatementExecution {
    return PreparedStatementExecution{ *this->m_DuckDbPreparedStatement, SourceLocation };
}

}
