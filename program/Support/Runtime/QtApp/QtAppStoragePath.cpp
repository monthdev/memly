#include "Support/Runtime/QtApp/QtAppStoragePath.hpp"

#include <qdir.h>
#include <qstandardpaths.h>

#include <string>

namespace Support::Runtime::QtApp {
namespace {
[[nodiscard]] auto u_EnsureDirectory(const QString& Directory) -> std::string {
    QDir().mkpath(Directory);
    return std::string{ Directory.toStdString() };
}

[[nodiscard]] auto u_BaseDirectoryPath() -> std::string {
    return std::string{ u_EnsureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)) };
}
}

[[nodiscard]] auto AudioDirectoryPath() -> std::string {
    return std::string{ u_EnsureDirectory(QString::fromStdString(u_BaseDirectoryPath() + "/Audio")) };
}

[[nodiscard]] auto ExceptionLogFilePath() -> std::string {
    return std::string{ u_BaseDirectoryPath() + "/exception.log" };
}

[[nodiscard]] auto DatabaseFilePath() -> std::string {
    return std::string{ u_BaseDirectoryPath() + "/memly.duckdb" };
}
}
