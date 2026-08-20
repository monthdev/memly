module;

#include <cstddef>
#include <exception>
#include <fstream>
#include <ios>
#include <span>
#include <string_view>

#if defined(_WIN32)
#include <io.h>

#include <algorithm>
#include <cstdio>
#include <limits>
#else
#include <unistd.h>
#endif

module Memly.Exception;

import Memly.QtApp;

namespace Memly::Exception {
namespace {
[[nodiscard]] auto u_WriteStdErrBytes(const char* const Bytes, const std::size_t ByteCount) noexcept -> std::ptrdiff_t {
#if defined(_WIN32)
    return std::ptrdiff_t{ ::_write(
        ::_fileno(stderr), Bytes, static_cast<unsigned int>(std::min(ByteCount, static_cast<std::size_t>(std::numeric_limits<int>::max())))) };
#else
    return std::ptrdiff_t{ ::write(STDERR_FILENO, Bytes, ByteCount) };
#endif
}

void u_WriteToStdErr(const std::string_view ErrorMessage) noexcept {
    const std::span<const char> ErrorMessageSpan{ ErrorMessage };
    std::size_t TotalWrittenSize{ 0 };
    std::ptrdiff_t WrittenSize{ 0 };
    while (TotalWrittenSize < ErrorMessageSpan.size() and
           (WrittenSize = u_WriteStdErrBytes(ErrorMessageSpan.subspan(TotalWrittenSize).data(), ErrorMessageSpan.size() - TotalWrittenSize)) > 0) {
        TotalWrittenSize += static_cast<std::size_t>(WrittenSize);
    }
    if (TotalWrittenSize == ErrorMessageSpan.size()) {
        static_cast<void>(u_WriteStdErrBytes("\n", 1));
    }
}

}

void LogException(const std::string_view ExceptionMessage) noexcept {
    try {
        u_WriteToStdErr(ExceptionMessage);
        std::ofstream ExceptionLogFile{};
        ExceptionLogFile.exceptions(std::ios::failbit bitor std::ios::badbit);
        ExceptionLogFile.open(QtApp::ExceptionLogFilePath());
        ExceptionLogFile.write(ExceptionMessage.data(), static_cast<std::streamsize>(ExceptionMessage.size()));
    } catch (const std::exception& CaughtException) { u_WriteToStdErr(CaughtException.what()); } catch (...) {
        u_WriteToStdErr("Exception log file write failed");
    }
}

}
