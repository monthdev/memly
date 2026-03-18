#include "SqlResource.hpp"

#include <QResource>

#include "Support/Fatal.hpp"

namespace Sql {
static std::string ReadResourceBytes(const char* ResourcePath) {
    QResource Resource{ ResourcePath };
    if (!Resource.isValid()) {
        Support::ThrowError();
    }
    return std::string{ reinterpret_cast<const char*>(Resource.data()),
                        static_cast<size_t>(Resource.size()) };
}

std::string InitialSchemaSql() {
    return ReadResourceBytes(":/Sql/Migrations/1_InitialSchema.sql");
}

std::string CreateDeckSql() {
    return ReadResourceBytes(":/Sql/Statements/CreateDeck.sql");
}

std::string ReadDeckTableSql() {
    return ReadResourceBytes(":/Sql/Statements/ReadDeckTable.sql");
}

std::string UpdateDeckSql() {
    return ReadResourceBytes(":/Sql/Statements/UpdateDeck.sql");
}

std::string DeleteDeckSql() {
    return ReadResourceBytes(":/Sql/Statements/DeleteDeck.sql");
}
}
