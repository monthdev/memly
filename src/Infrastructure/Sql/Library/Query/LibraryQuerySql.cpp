#include "LibraryQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Library::Query {

[[nodiscard]] std::string ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql() {
    return ReadSqlResourceBytes(":/Sql/Library/Query/Statement/ReadNextLibraryInvalidationAtMillisecondsSinceEpoch.sql");
}
}
