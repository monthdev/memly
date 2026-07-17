#include "ReviewSessionMutationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::ReviewSession::Mutation {

[[nodiscard]] std::string CreateCustomReviewSessionSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateCustomReviewSession.sql");
}

[[nodiscard]] std::string CreateDefaultReviewSessionSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateDefaultReviewSession.sql");
}

[[nodiscard]] std::string CreateCustomReviewSessionDeckSelectionSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateCustomReviewSessionDeckSelection.sql");
}

[[nodiscard]] std::string RenameReviewSessionSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/RenameReviewSession.sql");
}

[[nodiscard]] std::string UpdateReviewSessionToDefaultSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionToDefault.sql");
}

[[nodiscard]] std::string UpdateReviewSessionToCustomSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionToCustom.sql");
}

[[nodiscard]] std::string UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] std::string DeleteCustomReviewSessionDeckSelectionsSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/DeleteCustomReviewSessionDeckSelections.sql");
}

[[nodiscard]] std::string DeleteReviewSessionSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/DeleteReviewSession.sql");
}
}
