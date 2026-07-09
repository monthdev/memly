#include "Support/Runtime/Crash.hpp"

#include <QSaveFile>
#include <format>
#include <iostream>
#include <source_location>
#include <stdexcept>

#include "Support/Runtime/AppStoragePath.hpp"

namespace Support::Runtime {
[[noreturn]] void ThrowError(const std::string_view ErrorMessage, const std::source_location& SourceLocation) {
    std::string What{ std::format(
        "Exception thrown in {}, {}, line {}:\n\t{}", SourceLocation.file_name(), SourceLocation.function_name(), SourceLocation.line(), ErrorMessage) };
    throw std::runtime_error(What);
}

void LogError(const std::string_view What) {
    QSaveFile CrashLogFile{ CrashLogFilePath() };
    if (CrashLogFile.open(QIODevice::WriteOnly bitor QIODevice::Text)) {
        CrashLogFile.write(What.data(), static_cast<qint64>(What.size()));
        CrashLogFile.commit();
    } else {
        std::cerr << "Failed to write error to log file";
    }
}
}
