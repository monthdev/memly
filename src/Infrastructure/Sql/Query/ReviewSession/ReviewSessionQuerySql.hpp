#pragma once

#include <string>

namespace Infrastructure::Sql::Query::ReviewSession {

[[nodiscard]] std::string ReadReviewSessionListSql();

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql();

[[nodiscard]] std::string ReadReviewSessionIdByReviewSessionDefinitionKeySql();
}
