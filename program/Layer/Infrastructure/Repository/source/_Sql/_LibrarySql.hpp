// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <string>

namespace Memly::Repository {

[[nodiscard]] auto i_SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string;
}
#endif
