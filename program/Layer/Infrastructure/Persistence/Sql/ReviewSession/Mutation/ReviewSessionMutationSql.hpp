// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <string>

namespace Layer::Infrastructure::Persistence::Sql::ReviewSession::Mutation {

[[nodiscard]] auto CreateCustomReviewSessionSql() -> std::string;

[[nodiscard]] auto CreateDefaultReviewSessionSql() -> std::string;

[[nodiscard]] auto CreateCustomReviewSessionDeckSelectionSql() -> std::string;

[[nodiscard]] auto RenameReviewSessionSql() -> std::string;

[[nodiscard]] auto UpdateReviewSessionToDefaultSql() -> std::string;

[[nodiscard]] auto UpdateReviewSessionToCustomSql() -> std::string;

[[nodiscard]] auto UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() -> std::string;

[[nodiscard]] auto DeleteCustomReviewSessionDeckSelectionsSql() -> std::string;

[[nodiscard]] auto DeleteReviewSessionSql() -> std::string;
}
#endif
