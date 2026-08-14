// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "_LibrarySql.hpp"

#include <string>

#include "Memly/QtApp/QtAppResourceBytes.hpp"

namespace Memly::Repository {

[[nodiscard]] auto SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string {
    return QtApp::ReadQtAppResourceBytes(":/Sql/Library/Select/SelectNextLibraryInvalidationAtMillisecondsSinceEpoch.sql");
}
}
#endif
