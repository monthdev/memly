#pragma once

#include <string>

namespace Infrastructure::Sql::ReviewSession::Query {

[[nodiscard]] std::string ReadReviewSessionListSql();

[[nodiscard]] std::string ReadDefaultReviewSessionIdByRootDeckIdSql();

[[nodiscard]] std::string ReadReviewSessionIdByReviewSessionDefinitionKeySql();
}
