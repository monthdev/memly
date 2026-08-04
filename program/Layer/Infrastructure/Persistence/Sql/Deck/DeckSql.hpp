#pragma once

#include <string>

namespace Layer::Infrastructure::Persistence::Sql::Deck {

[[nodiscard]] auto SelectDeckSnapshotRecordsSql() -> std::string;

[[nodiscard]] auto InsertRootDeckSql() -> std::string;

[[nodiscard]] auto InsertChildDeckSql() -> std::string;

[[nodiscard]] auto UpdateDeckParentToRootSql() -> std::string;

[[nodiscard]] auto UpdateDeckParentSql() -> std::string;

[[nodiscard]] auto UpdateDeckNameSql() -> std::string;

[[nodiscard]] auto DeleteDeckCardReviewsSql() -> std::string;

[[nodiscard]] auto DeleteDeckCardsSql() -> std::string;

[[nodiscard]] auto DeleteDeckSql() -> std::string;
}
