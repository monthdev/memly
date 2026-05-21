#include "SqlResourceBytes.hpp"

#include <QResource>

#include "Support/Fatal.hpp"

namespace Infrastructure::Sql {

[[nodiscard]] std::string ReadSqlResourceBytes(const char* SqlResourcePath) {
    QResource SqlResource{ SqlResourcePath };
    if (not SqlResource.isValid()) {
        Support::ThrowError("Error opening SQL file");
    }
    const QByteArray SqlResourceBytes{ SqlResource.uncompressedData() };
    if (SqlResourceBytes.isNull()) {
        Support::ThrowError("Error reading SQL resource bytes");
    }
    return std::string{ SqlResourceBytes.constData(), static_cast<std::size_t>(SqlResourceBytes.size()) };
}
}
