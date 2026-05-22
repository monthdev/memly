#include "QuerySqlResource.hpp"

#include "SqlResourceBytes.hpp"

namespace Infrastructure::Sql {

[[nodiscard]] std::string ReadDeckTreeSnapshotSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReadDeckTreeSnapshot.sql");
}

[[nodiscard]] std::string ReadNextLibraryRefreshAtMillisecondsSinceEpochSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReadNextLibraryRefreshAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] std::string ReadReviewSessionListSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReadReviewSessionList.sql");
}

[[nodiscard]] std::string ReadReviewSessionIdByDefinitionKeySql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReadReviewSessionIdByDefinitionKey.sql");
}

[[nodiscard]] std::string ReadReviewSessionNameAndDefinitionKeyByIdSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReadReviewSessionNameAndDefinitionKeyById.sql");
}

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReadDefaultReviewSessionIdByRootDeckId.sql");
}
}
