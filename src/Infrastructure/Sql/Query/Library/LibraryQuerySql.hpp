#pragma once

#include <string>

namespace Infrastructure::Sql::Query::Library {

[[nodiscard]] std::string ReadNextLibraryRefreshAtMillisecondsSinceEpochSql();
}
