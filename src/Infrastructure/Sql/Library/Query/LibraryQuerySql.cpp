#include "LibraryQuerySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Library::Query {

[[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Library/Query/Statement/ReadNextLibraryInvalidationAtMillisecondsSinceEpoch.sql");
}
}
