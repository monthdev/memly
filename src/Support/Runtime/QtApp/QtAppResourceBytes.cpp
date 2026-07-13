#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

#include <QByteArray>
#include <QResource>
#include <cassert>

namespace Support::Runtime::QtApp {
[[nodiscard]] std::string ReadQtAppResourceBytes(const std::string_view QtResourcePath) {
    QResource Resource{ QtResourcePath.data() };
    assert(Resource.isValid());
    const QByteArray ResourceBytes{ Resource.uncompressedData() };
    assert(not ResourceBytes.isNull());
    return std::string{ ResourceBytes.constData(), static_cast<std::size_t>(ResourceBytes.size()) };
}
}
