// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "LibrarySql.hpp"

#include <string>

#include "Memly/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Repository::Sql {

[[nodiscard]] auto SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string {
    return Support::QtApp::ReadQtAppResourceBytes(":/Sql/Library/Select/SelectNextLibraryInvalidationAtMillisecondsSinceEpoch.sql");
}
}
#endif
