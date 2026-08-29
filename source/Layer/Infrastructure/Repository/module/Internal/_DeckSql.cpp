module;

#include <string>

module Memly.Repository.Internal.DeckSql;

import Memly.QtApp.QtAppResourceBytes;

namespace Memly::Repository::Internal {

[[nodiscard]] std::string
SelectDeckSnapshotRecordsSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(
            ":/Sql/Deck/Select/SelectDeckSnapshotRecords.sql"
        ),
    };
}

[[nodiscard]] std::string
InsertRootDeckSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Insert/InsertRootDeck.sql"),
    };
}

[[nodiscard]] std::string
InsertChildDeckSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Insert/InsertChildDeck.sql"),
    };
}

[[nodiscard]] std::string
UpdateDeckParentToRootSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(
            ":/Sql/Deck/Update/UpdateDeckParentToRoot.sql"
        ),
    };
}

[[nodiscard]] std::string
UpdateDeckParentSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/UpdateDeckParent.sql"),
    };
}

[[nodiscard]] std::string
UpdateDeckNameSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Update/UpdateDeckName.sql"),
    };
}

[[nodiscard]] std::string
DeleteDeckCardReviewsSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(
            ":/Sql/Deck/Delete/DeleteDeckCardReviews.sql"
        ),
    };
}

[[nodiscard]] std::string
DeleteDeckCardsSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeckCards.sql"),
    };
}

[[nodiscard]] std::string
DeleteDeckSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Delete/DeleteDeck.sql"),
    };
}
}
