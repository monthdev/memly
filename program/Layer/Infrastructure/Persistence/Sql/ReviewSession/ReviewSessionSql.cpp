// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "ReviewSessionSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Persistence::Sql::ReviewSession {

[[nodiscard]] auto ReadReviewSessionListRowsSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/ReadReviewSessionListRows.sql");
}

[[nodiscard]] auto ReadDefaultReviewSessionIdByRootDeckIdSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/ReadDefaultReviewSessionIdByRootDeckId.sql");
}

[[nodiscard]] auto ReadReviewSessionIdByReviewSessionDefinitionKeySql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/ReadReviewSessionIdByReviewSessionDefinitionKey.sql");
}

[[nodiscard]] auto CreateCustomReviewSessionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/CreateCustomReviewSession.sql");
}

[[nodiscard]] auto CreateDefaultReviewSessionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/CreateDefaultReviewSession.sql");
}

[[nodiscard]] auto CreateCustomReviewSessionDeckSelectionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/CreateCustomReviewSessionDeckSelection.sql");
}

[[nodiscard]] auto RenameReviewSessionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/RenameReviewSession.sql");
}

[[nodiscard]] auto UpdateReviewSessionToDefaultSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/UpdateReviewSessionToDefault.sql");
}

[[nodiscard]] auto UpdateReviewSessionToCustomSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/UpdateReviewSessionToCustom.sql");
}

[[nodiscard]] auto UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Update/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] auto DeleteCustomReviewSessionDeckSelectionsSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Delete/DeleteCustomReviewSessionDeckSelections.sql");
}

[[nodiscard]] auto DeleteReviewSessionSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Delete/DeleteReviewSession.sql");
}
}
#endif
