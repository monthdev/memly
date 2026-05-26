#include "SqlResourceBytes.hpp"

#include <QResource>

#include "Runtime/Crash.hpp"

namespace Infrastructure::Sql {

[[nodiscard]] std::string ReadSqlResourceBytes(const char* SqlResourcePath) {
    QResource SqlResource{ SqlResourcePath };
    if (not SqlResource.isValid()) {
        Runtime::ThrowError("Error opening SQL file");
    }
    const QByteArray SqlResourceBytes{ SqlResource.uncompressedData() };
    if (SqlResourceBytes.isNull()) {
        Runtime::ThrowError("Error reading SQL resource bytes");
    }
    return std::string{ SqlResourceBytes.constData(), static_cast<std::size_t>(SqlResourceBytes.size()) };
}
}
