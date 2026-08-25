export module Memly.Repository.Internal.ReviewSessionSql;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <string>

export namespace Memly::Repository::Internal {

[[nodiscard]] auto SelectReviewSessionListRowsSql() -> std::string;

[[nodiscard]] auto SelectDefaultReviewSessionIdByRootDeckIdSql() -> std::string;

[[nodiscard]] auto SelectReviewSessionIdByReviewSessionDefinitionKeySql() -> std::string;

[[nodiscard]] auto InsertCustomReviewSessionSql() -> std::string;

[[nodiscard]] auto InsertDefaultReviewSessionSql() -> std::string;

[[nodiscard]] auto InsertCustomReviewSessionDeckSelectionSql() -> std::string;

[[nodiscard]] auto UpdateReviewSessionNameSql() -> std::string;

[[nodiscard]] auto UpdateReviewSessionToDefaultSql() -> std::string;

[[nodiscard]] auto UpdateReviewSessionToCustomSql() -> std::string;

[[nodiscard]] auto UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() -> std::string;

[[nodiscard]] auto DeleteCustomReviewSessionDeckSelectionsSql() -> std::string;

[[nodiscard]] auto DeleteReviewSessionSql() -> std::string;
}
#endif
