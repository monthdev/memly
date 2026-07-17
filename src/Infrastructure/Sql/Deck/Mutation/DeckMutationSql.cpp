#include "DeckMutationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Deck::Mutation {

[[nodiscard]] std::string CreateRootDeckSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/CreateRootDeck.sql");
}

[[nodiscard]] std::string CreateChildDeckSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/CreateChildDeck.sql");
}

[[nodiscard]] std::string MoveDeckSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/MoveDeck.sql");
}

[[nodiscard]] std::string RenameDeckSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/RenameDeck.sql");
}

[[nodiscard]] std::string DeleteDeckCardReviewsSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeckCardReviews.sql");
}

[[nodiscard]] std::string DeleteDeckCardsSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeckCards.sql");
}

[[nodiscard]] std::string DeleteDeckSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeck.sql");
}
}
