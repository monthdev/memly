#include "Support/Runtime/ExceptionBoundary.hpp"

#include <cstddef>
#include <exception>
#include <fstream>
#include <ios>
#include <string_view>

#if defined(_WIN32)
#include <io.h>

#include <algorithm>
#include <cstdio>
#include <limits>
#else
#include <unistd.h>
#endif

#include "Support/Runtime/QtApp/QtAppStoragePath.hpp"

namespace Support::Runtime {
namespace {
[[nodiscard]] auto a_WriteStdErrBytes(const char* const Bytes, const std::size_t ByteCount) noexcept -> std::ptrdiff_t {
#if defined(_WIN32)
    return ::_write(::_fileno(stderr), Bytes, static_cast<unsigned int>(std::min(ByteCount, static_cast<std::size_t>(std::numeric_limits<int>::max()))));
#else
    return ::write(STDERR_FILENO, Bytes, ByteCount);
#endif
}

void a_WriteToStdErr(const std::string_view ErrorMessage) noexcept {
    std::size_t TotalWrittenSize{ 0 };
    std::ptrdiff_t WrittenSize{ 0 };
    while (TotalWrittenSize < ErrorMessage.size() and
           (WrittenSize = a_WriteStdErrBytes(ErrorMessage.data() + TotalWrittenSize, ErrorMessage.size() - TotalWrittenSize)) > 0) {
        TotalWrittenSize += static_cast<std::size_t>(WrittenSize);
    }
    if (TotalWrittenSize == ErrorMessage.size()) {
        static_cast<void>(a_WriteStdErrBytes("\n", 1));
    }
}

}

void LogException(const std::string_view ExceptionMessage) noexcept {
    try {
        a_WriteToStdErr(ExceptionMessage);
        std::ofstream ExceptionLogFile{};
        ExceptionLogFile.exceptions(std::ios::failbit bitor std::ios::badbit);
        ExceptionLogFile.open(QtApp::ExceptionLogFilePath());
        ExceptionLogFile.write(ExceptionMessage.data(), static_cast<std::streamsize>(ExceptionMessage.size()));
    } catch (const std::exception& Exception) { a_WriteToStdErr(Exception.what()); } catch (...) {
        a_WriteToStdErr("Failed to write exception log file");
    }
}

}
