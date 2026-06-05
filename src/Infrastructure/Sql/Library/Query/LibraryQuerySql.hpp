#pragma once

#include <string>

namespace Infrastructure::Sql::Library::Query {

[[nodiscard]] std::string ReadNextLibraryRefreshAtMillisecondsSinceEpochSql();
}
