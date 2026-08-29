module;

#include <qdir.h>
#include <qstandardpaths.h>
#include <qstring.h>

#include <string>

module Memly.QtApp.QtAppStoragePath;

namespace Memly::QtApp {
namespace {
[[nodiscard]] std::string
u_EnsureDirectory(const QString& Directory) {
    QDir().mkpath(Directory);
    return std::string{ Directory.toStdString() };
}

[[nodiscard]] std::string
u_BaseDirectoryPath() {
    return std::string{
        u_EnsureDirectory(
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        ),
    };
}
}

[[nodiscard]] std::string
AudioDirectoryPath() {
    return std::string{
        u_EnsureDirectory(
            QString::fromStdString(u_BaseDirectoryPath() + "/Audio")
        ),
    };
}

[[nodiscard]] std::string
ExceptionLogFilePath() {
    return std::string{ u_BaseDirectoryPath() + "/exception.log" };
}

[[nodiscard]] std::string
DatabaseFilePath() {
    return std::string{ u_BaseDirectoryPath() + "/memly.duckdb" };
}
}
