module;

#include <string>

export module Memly.Repository.Internal.DeckSql;

export namespace Memly::Repository::Internal {

[[nodiscard]] std::string
SelectDeckSnapshotRecordsSql();

[[nodiscard]] std::string
InsertRootDeckSql();

[[nodiscard]] std::string
InsertChildDeckSql();

[[nodiscard]] std::string
UpdateDeckParentToRootSql();

[[nodiscard]] std::string
UpdateDeckParentSql();

[[nodiscard]] std::string
UpdateDeckNameSql();

[[nodiscard]] std::string
DeleteDeckCardReviewsSql();

[[nodiscard]] std::string
DeleteDeckCardsSql();

[[nodiscard]] std::string
DeleteDeckSql();
}
