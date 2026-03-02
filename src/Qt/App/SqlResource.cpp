#include "SqlResource.hpp"

#include <QResource>

#include "Utility/Unrecoverable.hpp"

static std::string ReadResourceBytes(const char* ResourcePath) {
    QResource Resource{ ResourcePath };
    if (!Resource.isValid()) {
        Unrecoverable::Throw();
    }
    return std::string{ reinterpret_cast<const char*>(Resource.data()),
                        static_cast<size_t>(Resource.size()) };
}

std::string Qt::App::SqlResource::SchemaSql() {
    return ReadResourceBytes(":/Sql/Migrations/Schema.sql");
}
