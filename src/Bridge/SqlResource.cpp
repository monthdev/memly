#include "SqlResource.hpp"

#include <QResource>

static std::string ReadResourceBytes(const char* ResourcePath) {
    QResource Resource{ ResourcePath };
    if (!Resource.isValid()) {
        return {};
    }
    return std::string{ reinterpret_cast<const char*>(Resource.data()),
                        static_cast<size_t>(Resource.size()) };
}

std::string SchemaSql() {
    return ReadResourceBytes(":/Sql/Migrations/Schema.sql");
}
