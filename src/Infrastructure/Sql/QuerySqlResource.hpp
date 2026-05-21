#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] std::string ReadDeckTreeSnapshotSql();

[[nodiscard]] std::string ReadNextLibraryRefreshAtMillisecondsSinceEpochSql();

[[nodiscard]] std::string ReadReviewSessionListSql();

[[nodiscard]] std::string ReadReviewSessionIdByDefinitionKeySql();

[[nodiscard]] std::string ReadReviewSessionNameAndDefinitionKeyByIdSql();

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql();

[[nodiscard]] std::string ValidateDeckExistsSql();

[[nodiscard]] std::string ValidateParentDeckExistsSql();
}
