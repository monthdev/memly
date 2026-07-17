#include "LibraryQuerySql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Library::Query {

[[nodiscard]] std::string ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Library/Query/Statement/ReadNextLibraryInvalidationAtMillisecondsSinceEpoch.sql");
}
}
