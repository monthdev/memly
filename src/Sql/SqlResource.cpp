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

std::string InitialSchema() {
    return ReadResourceBytes(":/Sql/Migrations/1_InitialSchema.sql");
}

std::string AddDeckStatement() {
    return ReadResourceBytes(":/Sql/Statements/AddDeck.sql");
}
}
