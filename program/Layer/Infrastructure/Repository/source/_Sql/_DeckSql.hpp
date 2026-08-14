#pragma once

#include <string>

namespace Memly::Repository {

[[nodiscard]] auto i_SelectDeckSnapshotRecordsSql() -> std::string;

[[nodiscard]] auto i_InsertRootDeckSql() -> std::string;

[[nodiscard]] auto i_InsertChildDeckSql() -> std::string;

[[nodiscard]] auto i_UpdateDeckParentToRootSql() -> std::string;

[[nodiscard]] auto i_UpdateDeckParentSql() -> std::string;

[[nodiscard]] auto i_UpdateDeckNameSql() -> std::string;

[[nodiscard]] auto i_DeleteDeckCardReviewsSql() -> std::string;

[[nodiscard]] auto i_DeleteDeckCardsSql() -> std::string;

[[nodiscard]] auto i_DeleteDeckSql() -> std::string;
}
