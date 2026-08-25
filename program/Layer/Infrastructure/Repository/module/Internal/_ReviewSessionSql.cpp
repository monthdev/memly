module Memly.Repository.Internal.ReviewSessionSql;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <string>

namespace Memly::Repository::Internal {

[[nodiscard]] auto SelectReviewSessionListRowsSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/SelectReviewSessionListRows.sql");
}

[[nodiscard]] auto SelectDefaultReviewSessionIdByRootDeckIdSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/SelectDefaultReviewSessionIdByRootDeckId.sql");
}

[[nodiscard]] auto SelectReviewSessionIdByReviewSessionDefinitionKeySql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/SelectReviewSessionIdByReviewSessionDefinitionKey.sql");
}

[[nodiscard]] auto InsertCustomReviewSessionSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/InsertCustomReviewSession.sql");
}

[[nodiscard]] auto InsertDefaultReviewSessionSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/InsertDefaultReviewSession.sql");
}

[[nodiscard]] auto InsertCustomReviewSessionDeckSelectionSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/InsertCustomReviewSessionDeckSelection.sql");
}

[[nodiscard]] auto UpdateReviewSessionNameSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/UpdateReviewSessionName.sql");
}

[[nodiscard]] auto UpdateReviewSessionToDefaultSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/UpdateReviewSessionToDefault.sql");
}

[[nodiscard]] auto UpdateReviewSessionToCustomSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/UpdateReviewSessionToCustom.sql");
}

[[nodiscard]] auto UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Update/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] auto DeleteCustomReviewSessionDeckSelectionsSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Delete/DeleteCustomReviewSessionDeckSelections.sql");
}

[[nodiscard]] auto DeleteReviewSessionSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Delete/DeleteReviewSession.sql");
}
}
#endif
