// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "_ReviewSessionSql.hpp"

#include <string>

#include "Memly/QtApp/QtAppResourceBytes.hpp"

namespace Memly::Repository {

[[nodiscard]] auto i_SelectReviewSessionListRowsSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/SelectReviewSessionListRows.sql");
}

[[nodiscard]] auto i_SelectDefaultReviewSessionIdByRootDeckIdSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/SelectDefaultReviewSessionIdByRootDeckId.sql");
}

[[nodiscard]] auto i_SelectReviewSessionIdByReviewSessionDefinitionKeySql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Select/SelectReviewSessionIdByReviewSessionDefinitionKey.sql");
}

[[nodiscard]] auto i_InsertCustomReviewSessionSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/InsertCustomReviewSession.sql");
}

[[nodiscard]] auto i_InsertDefaultReviewSessionSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/InsertDefaultReviewSession.sql");
}

[[nodiscard]] auto i_InsertCustomReviewSessionDeckSelectionSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Insert/InsertCustomReviewSessionDeckSelection.sql");
}

[[nodiscard]] auto i_UpdateReviewSessionNameSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/UpdateReviewSessionName.sql");
}

[[nodiscard]] auto i_UpdateReviewSessionToDefaultSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/UpdateReviewSessionToDefault.sql");
}

[[nodiscard]] auto i_UpdateReviewSessionToCustomSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Update/UpdateReviewSessionToCustom.sql");
}

[[nodiscard]] auto i_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Update/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] auto i_DeleteCustomReviewSessionDeckSelectionsSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Delete/DeleteCustomReviewSessionDeckSelections.sql");
}

[[nodiscard]] auto i_DeleteReviewSessionSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/ReviewSession/Delete/DeleteReviewSession.sql");
}
}
#endif
