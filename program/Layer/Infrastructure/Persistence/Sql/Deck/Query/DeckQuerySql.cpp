#include "DeckQuerySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Persistence::Sql::Deck::Query {

[[nodiscard]] auto ReadDeckSnapshotRecordsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Deck/Query/Statement/ReadDeckSnapshotRecords.sql") };
}
}
