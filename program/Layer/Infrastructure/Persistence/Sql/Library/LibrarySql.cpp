// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "LibrarySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Persistence::Sql::Library {

[[nodiscard]] auto SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Library/Select/SelectNextLibraryInvalidationAtMillisecondsSinceEpoch.sql");
}
}
#endif
