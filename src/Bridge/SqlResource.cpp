#include "SqlResource.hpp"

#include <QResource>

#include "Bridge/Throw.hpp"

static std::string ReadResourceBytes(const char* ResourcePath) {
    QResource Resource{ ResourcePath };
    if (!Resource.isValid()) {
        Throw();
    }
    return std::string{ reinterpret_cast<const char*>(Resource.data()),
                        static_cast<size_t>(Resource.size()) };
}

std::string InitialSchemaSql() {
    return ReadResourceBytes(":/Sql/Migrations/1_InitialSchema.sql");
}
