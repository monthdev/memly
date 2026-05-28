#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] std::string ReadDeckTreeSnapshotSql();

[[nodiscard]] std::string ReadNextLibraryRefreshAtMillisecondsSinceEpochSql();

[[nodiscard]] std::string ReadReviewSessionListSql();

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql();

[[nodiscard]] std::string ReadReviewSessionIdByReviewSessionDefinitionKeySql();
}
