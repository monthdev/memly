#include "Support/Runtime/QtApp/QtAppStoragePath.hpp"

#include <qdir.h>
#include <qstandardpaths.h>

#include <string>

namespace Support::Runtime::QtApp {
namespace {
[[nodiscard]] auto a_EnsureDirectory(const QString& Directory) -> std::string {
    QDir().mkpath(Directory);
    return Directory.toStdString();
}

[[nodiscard]] auto a_BaseDirectoryPath() -> std::string {
    return a_EnsureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
}
}

[[nodiscard]] auto AudioDirectoryPath() -> std::string {
    return a_EnsureDirectory(QString::fromStdString(a_BaseDirectoryPath() + "/Audio"));
}

[[nodiscard]] auto ExceptionLogFilePath() -> std::string {
    return a_BaseDirectoryPath() + "/exception.log";
}

[[nodiscard]] auto DatabaseFilePath() -> std::string {
    return a_BaseDirectoryPath() + "/memly.duckdb";
}
}
