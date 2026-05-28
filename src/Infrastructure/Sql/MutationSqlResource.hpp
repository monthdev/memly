#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] std::string CreateRootDeckSql();

[[nodiscard]] std::string CreateChildDeckSql();

[[nodiscard]] std::string MoveDeckSql();

[[nodiscard]] std::string RenameDeckSql();

[[nodiscard]] std::string CreateCustomReviewSessionSql();

[[nodiscard]] std::string CreateDefaultReviewSessionSql();

[[nodiscard]] std::string CreateCustomReviewSessionDeckSelectionSql();

[[nodiscard]] std::string RenameReviewSessionSql();

[[nodiscard]] std::string UpdateReviewSessionToDefaultSql();

[[nodiscard]] std::string UpdateReviewSessionToCustomSql();

[[nodiscard]] std::string UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql();

[[nodiscard]] std::string DeleteDeckCardReviewsSql();

[[nodiscard]] std::string DeleteDeckCardsSql();

[[nodiscard]] std::string DeleteDeckSql();

[[nodiscard]] std::string DeleteCustomReviewSessionDeckSelectionsSql();

[[nodiscard]] std::string DeleteReviewSessionSql();
}
