export module Memly.Repository.Internal.LibrarySql;

// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <string>

export namespace Memly::Repository::Internal {

[[nodiscard]] std::string
SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql();
}
#endif
