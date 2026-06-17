#include "DeckQuerySql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Deck::Query {

[[nodiscard]] std::string CheckDeckIdExistsSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Query/Statement/CheckDeckIdExists.sql");
}

[[nodiscard]] std::string ReadDeckTreeSnapshotNodesSql() {
    return ReadSqlResourceBytes(":/Sql/Deck/Query/Statement/ReadDeckTreeSnapshotNodes.sql");
}
}
