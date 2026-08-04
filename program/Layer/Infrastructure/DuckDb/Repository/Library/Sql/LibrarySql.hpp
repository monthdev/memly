// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <string>

namespace Layer::Infrastructure::DuckDb::Repository::Library::Sql {

[[nodiscard]] auto SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string;
}
#endif
