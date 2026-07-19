#pragma once

#include <string>

namespace Infrastructure::Sql::ReviewSession::Query {

[[nodiscard]] auto ReadReviewSessionListRowsSql() -> std::string;

[[nodiscard]] auto ReadDefaultReviewSessionIdByRootDeckIdSql() -> std::string;

[[nodiscard]] auto ReadReviewSessionIdByReviewSessionDefinitionKeySql() -> std::string;
}
