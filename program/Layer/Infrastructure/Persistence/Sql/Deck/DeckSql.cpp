#include "DeckSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Persistence::Sql::Deck {

[[nodiscard]] auto ReadDeckSnapshotRecordsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Select/ReadDeckSnapshotRecords.sql") };
}

[[nodiscard]] auto CreateRootDeckSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Insert/CreateRootDeck.sql") };
}

[[nodiscard]] auto CreateChildDeckSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Insert/CreateChildDeck.sql") };
}

[[nodiscard]] auto MoveDeckToRootSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/MoveDeckToRoot.sql") };
}

[[nodiscard]] auto MoveDeckUnderParentSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/MoveDeckUnderParent.sql") };
}

[[nodiscard]] auto RenameDeckSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/RenameDeck.sql") };
}

[[nodiscard]] auto DeleteDeckCardReviewsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeckCardReviews.sql") };
}

[[nodiscard]] auto DeleteDeckCardsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeckCards.sql") };
}

[[nodiscard]] auto DeleteDeckSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeck.sql") };
}
}
