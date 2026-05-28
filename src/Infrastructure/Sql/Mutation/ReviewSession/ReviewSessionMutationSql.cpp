#include "ReviewSessionMutationSql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Mutation::ReviewSession {

[[nodiscard]] std::string CreateCustomReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/CreateCustomReviewSession.sql");
}

[[nodiscard]] std::string CreateDefaultReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/CreateDefaultReviewSession.sql");
}

[[nodiscard]] std::string CreateCustomReviewSessionDeckSelectionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/CreateCustomReviewSessionDeckSelection.sql");
}

[[nodiscard]] std::string RenameReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/RenameReviewSession.sql");
}

[[nodiscard]] std::string UpdateReviewSessionToDefaultSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/UpdateReviewSessionToDefault.sql");
}

[[nodiscard]] std::string UpdateReviewSessionToCustomSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/UpdateReviewSessionToCustom.sql");
}

[[nodiscard]] std::string UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] std::string DeleteCustomReviewSessionDeckSelectionsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/DeleteCustomReviewSessionDeckSelections.sql");
}

[[nodiscard]] std::string DeleteReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/ReviewSession/Statement/DeleteReviewSession.sql");
}
}
