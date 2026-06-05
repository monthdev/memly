#include "DeckMutationSql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Deck::Mutation {

[[nodiscard]] std::string CreateRootDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Mutation/Statement/CreateRootDeck.sql");
}

[[nodiscard]] std::string CreateChildDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Mutation/Statement/CreateChildDeck.sql");
}

[[nodiscard]] std::string MoveDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Mutation/Statement/MoveDeck.sql");
}

[[nodiscard]] std::string RenameDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Mutation/Statement/RenameDeck.sql");
}

[[nodiscard]] std::string DeleteDeckCardReviewsSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeckCardReviews.sql");
}

[[nodiscard]] std::string DeleteDeckCardsSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeckCards.sql");
}

[[nodiscard]] std::string DeleteDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeck.sql");
}
}
