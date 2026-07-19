#include "DeckMutationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Deck::Mutation {

[[nodiscard]] auto CreateRootDeckSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/CreateRootDeck.sql");
}

[[nodiscard]] auto CreateChildDeckSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/CreateChildDeck.sql");
}

[[nodiscard]] auto MoveDeckSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/MoveDeck.sql");
}

[[nodiscard]] auto RenameDeckSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/RenameDeck.sql");
}

[[nodiscard]] auto DeleteDeckCardReviewsSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeckCardReviews.sql");
}

[[nodiscard]] auto DeleteDeckCardsSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeckCards.sql");
}

[[nodiscard]] auto DeleteDeckSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Mutation/Statement/DeleteDeck.sql");
}
}
