#pragma once

#include <string>

namespace Layer::Infrastructure::Sql::Deck::Query {

[[nodiscard]] auto ReadDeckSnapshotRecordsSql() -> std::string;
}
