#include "DeckQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Query::Deck {

[[nodiscard]] std::string ReadDeckTreeSnapshotSql() {
    return ReadSqlResourceBytes(":/Sql/Query/Deck/Statement/ReadDeckTreeSnapshot.sql");
}
}
