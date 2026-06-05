#include "DeckQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Deck::Query {

[[nodiscard]] std::string ReadDeckTreeSnapshotSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Query/Statement/ReadDeckTreeSnapshot.sql");
}
}
