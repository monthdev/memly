#pragma once

#include <string>

namespace Infrastructure::Sql::Mutation::ReviewSession {

[[nodiscard]] std::string CreateCustomReviewSessionSql();

[[nodiscard]] std::string CreateDefaultReviewSessionSql();

[[nodiscard]] std::string CreateCustomReviewSessionDeckSelectionSql();

[[nodiscard]] std::string RenameReviewSessionSql();

[[nodiscard]] std::string UpdateReviewSessionToDefaultSql();

[[nodiscard]] std::string UpdateReviewSessionToCustomSql();

[[nodiscard]] std::string UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql();

[[nodiscard]] std::string DeleteCustomReviewSessionDeckSelectionsSql();

[[nodiscard]] std::string DeleteReviewSessionSql();
}
