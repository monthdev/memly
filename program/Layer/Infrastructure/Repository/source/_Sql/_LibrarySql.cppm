export module Memly.Repository:LibrarySql;

// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <string>

export namespace Memly::Repository {

[[nodiscard]] auto i_SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string;
}
#endif
