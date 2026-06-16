#pragma once

#include <string>

namespace Infrastructure::Sql::ReviewSession::Query {

[[nodiscard]] std::string ReadReviewSessionListRowsSql();

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql();

[[nodiscard]] std::string ReadReviewSessionIdByReviewSessionDefinitionKeySql();
}
