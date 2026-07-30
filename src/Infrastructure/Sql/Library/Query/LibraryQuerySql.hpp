// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <string>

namespace Infrastructure::Sql::Library::Query {

[[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string;
}
#endif
