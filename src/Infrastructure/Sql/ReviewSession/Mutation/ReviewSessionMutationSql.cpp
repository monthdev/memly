#include "ReviewSessionMutationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::ReviewSession::Mutation {

[[nodiscard]] auto CreateCustomReviewSessionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateCustomReviewSession.sql");
}

[[nodiscard]] auto CreateDefaultReviewSessionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateDefaultReviewSession.sql");
}

[[nodiscard]] auto CreateCustomReviewSessionDeckSelectionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/CreateCustomReviewSessionDeckSelection.sql");
}

[[nodiscard]] auto RenameReviewSessionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/RenameReviewSession.sql");
}

[[nodiscard]] auto UpdateReviewSessionToDefaultSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionToDefault.sql");
}

[[nodiscard]] auto UpdateReviewSessionToCustomSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionToCustom.sql");
}

[[nodiscard]] auto UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Mutation/Statement/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] auto DeleteCustomReviewSessionDeckSelectionsSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/DeleteCustomReviewSessionDeckSelections.sql");
}

[[nodiscard]] auto DeleteReviewSessionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Mutation/Statement/DeleteReviewSession.sql");
}
}
