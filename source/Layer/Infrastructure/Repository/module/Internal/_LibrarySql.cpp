module Memly.Repository.Internal.LibrarySql;

// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <string>

namespace Memly::Repository::Internal {

[[nodiscard]] auto SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/Library/Select/SelectNextLibraryInvalidationAtMillisecondsSinceEpoch.sql");
}
}
#endif
