// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "LibraryQuerySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Sql::Library::Query {

[[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Library/Query/Statement/ReadNextLibraryInvalidationAtMillisecondsSinceEpoch.sql");
}
}
#endif
