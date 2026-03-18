#include "Fatal.hpp"

#include <QSaveFile>
#include <format>
#include <iostream>
#include <source_location>

#include "App/AppData.hpp"

namespace Support {
[[noreturn]] void ThrowError(const std::string& Message,
                             const std::source_location& SourceLocation) {
    std::string What{ std::format("Exception thrown in {}, {}, line {}:\n\t{}",
                                  SourceLocation.file_name(),
                                  SourceLocation.function_name(),
                                  SourceLocation.line(),
                                  Message) };
    throw std::runtime_error(What);
}

void LogError(const std::string& What) {
    QSaveFile CrashLogFile{ App::CrashLogFilePath() };
    if (CrashLogFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        CrashLogFile.write(What.data());
        CrashLogFile.commit();
    } else {
        std::cerr << "Failed to write error to log file";
    }
}
}
