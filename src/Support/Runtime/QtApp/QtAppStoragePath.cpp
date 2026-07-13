#include "Support/Runtime/QtApp/QtAppStoragePath.hpp"

#include <QDir>
#include <QStandardPaths>

namespace Support::Runtime::QtApp {
namespace {
[[nodiscard]] std::string a_EnsureDirectory(const QString& Directory) {
    QDir().mkpath(Directory);
    return Directory.toStdString();
}

[[nodiscard]] std::string a_BaseDirectoryPath() {
    return a_EnsureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
}
}

[[nodiscard]] std::string AudioDirectoryPath() {
    return a_EnsureDirectory(QString::fromStdString(a_BaseDirectoryPath() + "/Audio"));
}

[[nodiscard]] std::string ExceptionLogFilePath() {
    return a_BaseDirectoryPath() + "/exception.log";
}

[[nodiscard]] std::string DatabaseFilePath() {
    return a_BaseDirectoryPath() + "/memly.duckdb";
}
}
