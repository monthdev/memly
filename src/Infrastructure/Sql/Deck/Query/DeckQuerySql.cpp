#include "DeckQuerySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Deck::Query {

[[nodiscard]] std::string ReadDeckSnapshotRecordsSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Query/Statement/ReadDeckSnapshotRecords.sql");
}
}
