#include "ReviewSessionQuerySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::ReviewSession::Query {

[[nodiscard]] std::string ReadReviewSessionListRowsSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadReviewSessionListRows.sql");
}

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadDefaultReviewSessionIdByRootDeckId.sql");
}

[[nodiscard]] std::string ReadReviewSessionIdByReviewSessionDefinitionKeySql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Query/Statement/ReadReviewSessionIdByReviewSessionDefinitionKey.sql");
}
}
