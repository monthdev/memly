#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] std::string CreateRootDeckSql();

[[nodiscard]] std::string CreateChildDeckSql();

[[nodiscard]] std::string MoveDeckSql();

[[nodiscard]] std::string RenameDeckSql();

[[nodiscard]] std::string CreateReviewSessionSql();

[[nodiscard]] std::string CreateDefaultReviewSessionSql();

[[nodiscard]] std::string CreateReviewSessionDeckSelectionSql();

[[nodiscard]] std::string CreateDefaultReviewSessionDeckBindingSql();

[[nodiscard]] std::string UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql();

[[nodiscard]] std::string DeleteDeckCardReviewsSql();

[[nodiscard]] std::string DeleteDeckCardsSql();

[[nodiscard]] std::string DeleteDeckSql();

[[nodiscard]] std::string DeleteDefaultReviewSessionDeckBindingSql();

[[nodiscard]] std::string DeleteReviewSessionDeckSelectionsSql();

[[nodiscard]] std::string DeleteReviewSessionSql();
}
