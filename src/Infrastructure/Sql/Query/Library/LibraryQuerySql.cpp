#include "LibraryQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Query::Library {

[[nodiscard]] std::string ReadNextLibraryRefreshAtMillisecondsSinceEpochSql() {
    return ReadSqlResourceBytes(":/Sql/Query/Library/Statement/ReadNextLibraryRefreshAtMillisecondsSinceEpoch.sql");
}
}
