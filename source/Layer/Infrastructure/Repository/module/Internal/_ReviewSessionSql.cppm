export module Memly.Repository.Internal.ReviewSessionSql;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <string>

export namespace Memly::Repository::Internal {

[[nodiscard]] std::string
SelectReviewSessionListRowsSql();

[[nodiscard]] std::string
SelectDefaultReviewSessionIdByRootDeckIdSql();

[[nodiscard]] std::string
SelectReviewSessionIdByReviewSessionDefinitionKeySql();

[[nodiscard]] std::string
InsertCustomReviewSessionSql();

[[nodiscard]] std::string
InsertDefaultReviewSessionSql();

[[nodiscard]] std::string
InsertCustomReviewSessionDeckSelectionSql();

[[nodiscard]] std::string
UpdateReviewSessionNameSql();

[[nodiscard]] std::string
UpdateReviewSessionToDefaultSql();

[[nodiscard]] std::string
UpdateReviewSessionToCustomSql();

[[nodiscard]] std::string
UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql();

[[nodiscard]] std::string
DeleteCustomReviewSessionDeckSelectionsSql();

[[nodiscard]] std::string
DeleteReviewSessionSql();
}
#endif
