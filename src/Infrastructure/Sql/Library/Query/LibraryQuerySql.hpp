#pragma once

#include <string>

namespace Infrastructure::Sql::Library::Query {

[[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string;
}
