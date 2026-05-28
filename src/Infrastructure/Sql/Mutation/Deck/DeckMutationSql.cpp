#include "DeckMutationSql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Mutation::Deck {

[[nodiscard]] std::string CreateRootDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/Deck/Statement/CreateRootDeck.sql");
}

[[nodiscard]] std::string CreateChildDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/Deck/Statement/CreateChildDeck.sql");
}

[[nodiscard]] std::string MoveDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/Deck/Statement/MoveDeck.sql");
}

[[nodiscard]] std::string RenameDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/Deck/Statement/RenameDeck.sql");
}

[[nodiscard]] std::string DeleteDeckCardReviewsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/Deck/Statement/DeleteDeckCardReviews.sql");
}

[[nodiscard]] std::string DeleteDeckCardsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/Deck/Statement/DeleteDeckCards.sql");
}

[[nodiscard]] std::string DeleteDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/Deck/Statement/DeleteDeck.sql");
}
}
