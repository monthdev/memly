module Memly.Repository.Internal.ReviewSessionSql;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <string>

namespace Memly::Repository::Internal {

[[nodiscard]] std::string
SelectReviewSessionListRowsSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Select/SelectReviewSessionListRows.sql"
    );
}

[[nodiscard]] std::string
SelectDefaultReviewSessionIdByRootDeckIdSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Select/SelectDefaultReviewSessionIdByRootDeckId.sql"
    );
}

[[nodiscard]] std::string
SelectReviewSessionIdByReviewSessionDefinitionKeySql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Select/SelectReviewSessionIdByReviewSessionDefinitionKey.sql"
    );
}

[[nodiscard]] std::string
InsertCustomReviewSessionSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Insert/InsertCustomReviewSession.sql"
    );
}

[[nodiscard]] std::string
InsertDefaultReviewSessionSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Insert/InsertDefaultReviewSession.sql"
    );
}

[[nodiscard]] std::string
InsertCustomReviewSessionDeckSelectionSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Insert/InsertCustomReviewSessionDeckSelection.sql"
    );
}

[[nodiscard]] std::string
UpdateReviewSessionNameSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Update/UpdateReviewSessionName.sql"
    );
}

[[nodiscard]] std::string
UpdateReviewSessionToDefaultSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Update/UpdateReviewSessionToDefault.sql"
    );
}

[[nodiscard]] std::string
UpdateReviewSessionToCustomSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Update/UpdateReviewSessionToCustom.sql"
    );
}

[[nodiscard]] std::string
UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Update/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql"
    );
}

[[nodiscard]] std::string
DeleteCustomReviewSessionDeckSelectionsSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Delete/DeleteCustomReviewSessionDeckSelections.sql"
    );
}

[[nodiscard]] std::string
DeleteReviewSessionSql() {
    return QtApp::ReadQtAppResourceBytes(
        ":/Sql/ReviewSession/Delete/DeleteReviewSession.sql"
    );
}
}
#endif
