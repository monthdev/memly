#pragma once

#include <string>

namespace Infrastructure::Sql::ReviewSession::Mutation {

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
