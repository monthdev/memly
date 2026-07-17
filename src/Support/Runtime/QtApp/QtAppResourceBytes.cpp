#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

#include <qbytearray.h>
#include <qresource.h>

#include <cassert>
#include <cstddef>
#include <string>

namespace Support::Runtime::QtApp {
[[nodiscard]] std::string ReadQtAppResourceBytes(const char* const QtResourcePath) {
    const QResource Resource{ QtResourcePath };
    assert(Resource.isValid());
    const QByteArray ResourceBytes{ Resource.uncompressedData() };
    assert(not ResourceBytes.isNull());
    return std::string{ ResourceBytes.constData(), static_cast<std::size_t>(ResourceBytes.size()) };
}
}
