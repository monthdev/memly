#pragma once

#include <string>

namespace Infrastructure::Sql::Deck::Mutation {

[[nodiscard]] auto CreateRootDeckSql() -> std::string;

[[nodiscard]] auto CreateChildDeckSql() -> std::string;

[[nodiscard]] auto MoveDeckSql() -> std::string;

[[nodiscard]] auto RenameDeckSql() -> std::string;

[[nodiscard]] auto DeleteDeckCardReviewsSql() -> std::string;

[[nodiscard]] auto DeleteDeckCardsSql() -> std::string;

[[nodiscard]] auto DeleteDeckSql() -> std::string;
}
