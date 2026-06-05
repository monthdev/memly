#include "ReviewSessionMutationSql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::ReviewSession::Mutation {

[[nodiscard]] std::string CreateCustomReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateCustomReviewSession.sql");
}

[[nodiscard]] std::string CreateDefaultReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateDefaultReviewSession.sql");
}

[[nodiscard]] std::string CreateCustomReviewSessionDeckSelectionSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateCustomReviewSessionDeckSelection.sql");
}

[[nodiscard]] std::string RenameReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/RenameReviewSession.sql");
}

[[nodiscard]] std::string UpdateReviewSessionToDefaultSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionToDefault.sql");
}

[[nodiscard]] std::string UpdateReviewSessionToCustomSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionToCustom.sql");
}

[[nodiscard]] std::string UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] std::string DeleteCustomReviewSessionDeckSelectionsSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/DeleteCustomReviewSessionDeckSelections.sql");
}

[[nodiscard]] std::string DeleteReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/ReviewSession/Mutation/Statement/DeleteReviewSession.sql");
}
}
