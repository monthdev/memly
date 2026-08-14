#include "_DeckSql.hpp"

#include <string>

#include "Memly/QtApp/QtAppResourceBytes.hpp"

namespace Memly::Repository {

[[nodiscard]] auto SelectDeckSnapshotRecordsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Select/SelectDeckSnapshotRecords.sql") };
}

[[nodiscard]] auto InsertRootDeckSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Insert/InsertRootDeck.sql") };
}

[[nodiscard]] auto InsertChildDeckSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Insert/InsertChildDeck.sql") };
}

[[nodiscard]] auto UpdateDeckParentToRootSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/UpdateDeckParentToRoot.sql") };
}

[[nodiscard]] auto UpdateDeckParentSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/UpdateDeckParent.sql") };
}

[[nodiscard]] auto UpdateDeckNameSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/UpdateDeckName.sql") };
}

[[nodiscard]] auto DeleteDeckCardReviewsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeckCardReviews.sql") };
}

[[nodiscard]] auto DeleteDeckCardsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeckCards.sql") };
}

[[nodiscard]] auto DeleteDeckSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeck.sql") };
}
}
