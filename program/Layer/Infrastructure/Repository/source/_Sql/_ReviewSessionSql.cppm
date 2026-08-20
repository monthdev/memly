export module Memly.Repository:ReviewSessionSql;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <string>

export namespace Memly::Repository {

[[nodiscard]] auto i_SelectReviewSessionListRowsSql() -> std::string;

[[nodiscard]] auto i_SelectDefaultReviewSessionIdByRootDeckIdSql() -> std::string;

[[nodiscard]] auto i_SelectReviewSessionIdByReviewSessionDefinitionKeySql() -> std::string;

[[nodiscard]] auto i_InsertCustomReviewSessionSql() -> std::string;

[[nodiscard]] auto i_InsertDefaultReviewSessionSql() -> std::string;

[[nodiscard]] auto i_InsertCustomReviewSessionDeckSelectionSql() -> std::string;

[[nodiscard]] auto i_UpdateReviewSessionNameSql() -> std::string;

[[nodiscard]] auto i_UpdateReviewSessionToDefaultSql() -> std::string;

[[nodiscard]] auto i_UpdateReviewSessionToCustomSql() -> std::string;

[[nodiscard]] auto i_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() -> std::string;

[[nodiscard]] auto i_DeleteCustomReviewSessionDeckSelectionsSql() -> std::string;

[[nodiscard]] auto i_DeleteReviewSessionSql() -> std::string;
}
#endif
