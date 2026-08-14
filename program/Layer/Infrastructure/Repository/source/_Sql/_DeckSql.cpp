#include "_DeckSql.hpp"

#include <string>

#include "Memly/QtApp/QtAppResourceBytes.hpp"

namespace Memly::Repository {

[[nodiscard]] auto i_SelectDeckSnapshotRecordsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Select/SelectDeckSnapshotRecords.sql") };
}

[[nodiscard]] auto i_InsertRootDeckSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Insert/InsertRootDeck.sql") };
}

[[nodiscard]] auto i_InsertChildDeckSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Insert/InsertChildDeck.sql") };
}

[[nodiscard]] auto i_UpdateDeckParentToRootSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/UpdateDeckParentToRoot.sql") };
}

[[nodiscard]] auto i_UpdateDeckParentSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/UpdateDeckParent.sql") };
}

[[nodiscard]] auto i_UpdateDeckNameSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/UpdateDeckName.sql") };
}

[[nodiscard]] auto i_DeleteDeckCardReviewsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeckCardReviews.sql") };
}

[[nodiscard]] auto i_DeleteDeckCardsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeckCards.sql") };
}

[[nodiscard]] auto i_DeleteDeckSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeck.sql") };
}
}
