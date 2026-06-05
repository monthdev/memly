#include "LibraryQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Library::Query {

[[nodiscard]] std::string ReadNextLibraryRefreshAtMillisecondsSinceEpochSql() {
    return ReadSqlResourceBytes(":/Sql/Library/Query/Statement/ReadNextLibraryRefreshAtMillisecondsSinceEpoch.sql");
}
}
