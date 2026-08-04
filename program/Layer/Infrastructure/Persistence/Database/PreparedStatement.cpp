#include "Layer/Infrastructure/Persistence/Database/PreparedStatement.hpp"

#include <source_location>

#include "Layer/Infrastructure/Persistence/Database/PreparedStatementExecution.hpp"

namespace Layer::Infrastructure::Persistence::Database {

[[nodiscard]] auto PreparedStatement::Execute(const std::source_location& SourceLocation) noexcept -> PreparedStatementExecution {
    return PreparedStatementExecution{ *this->m_DuckDbPreparedStatement, SourceLocation };
}

}
