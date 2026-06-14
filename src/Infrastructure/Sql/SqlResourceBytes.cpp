#include "SqlResourceBytes.hpp"

#include <QResource>
#include <string_view>

#include "Runtime/Crash.hpp"

namespace Infrastructure::Sql {

[[nodiscard]] std::string ReadSqlResourceBytes(const std::string_view SqlResourcePath) {
    QResource SqlResource{ SqlResourcePath.data() };
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
