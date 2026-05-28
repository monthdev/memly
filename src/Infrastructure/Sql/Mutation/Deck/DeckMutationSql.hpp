#pragma once

#include <string>

namespace Infrastructure::Sql::Mutation::Deck {

[[nodiscard]] std::string CreateRootDeckSql();

[[nodiscard]] std::string CreateChildDeckSql();

[[nodiscard]] std::string MoveDeckSql();

[[nodiscard]] std::string RenameDeckSql();

[[nodiscard]] std::string DeleteDeckCardReviewsSql();

[[nodiscard]] std::string DeleteDeckCardsSql();

[[nodiscard]] std::string DeleteDeckSql();
}
