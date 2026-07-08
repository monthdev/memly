#include "DeckQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Deck::Query {

[[nodiscard]] std::string ReadDeckSnapshotRecordsSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Query/Statement/ReadDeckSnapshotRecords.sql");
}
}
