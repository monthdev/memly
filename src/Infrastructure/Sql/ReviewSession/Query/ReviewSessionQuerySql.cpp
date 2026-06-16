#include "ReviewSessionQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::ReviewSession::Query {

[[nodiscard]] std::string ReadReviewSessionListRowsSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadReviewSessionListRows.sql");
}

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadDefaultReviewSessionIdByRootDeckId.sql");
}

[[nodiscard]] std::string ReadReviewSessionIdByReviewSessionDefinitionKeySql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadReviewSessionIdByReviewSessionDefinitionKey.sql");
}
}
