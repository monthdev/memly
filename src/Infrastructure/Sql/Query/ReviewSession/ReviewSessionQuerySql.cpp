#include "ReviewSessionQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Query::ReviewSession {

[[nodiscard]] std::string ReadReviewSessionListSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReviewSession/Statement/ReadReviewSessionList.sql");
}

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReviewSession/Statement/ReadDefaultReviewSessionIdByRootDeckId.sql");
}

[[nodiscard]] std::string ReadReviewSessionIdByReviewSessionDefinitionKeySql() {
    return ReadSqlResourceBytes(":/Sql/Query/ReviewSession/Statement/ReadReviewSessionIdByReviewSessionDefinitionKey.sql");
}
}
