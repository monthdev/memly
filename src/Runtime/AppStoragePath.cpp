#include "Runtime/AppStoragePath.hpp"

#include <QDir>
#include <QStandardPaths>

namespace Runtime {
namespace {
[[nodiscard]] QString EnsureDirectory(const QString& Directory) {
    QDir().mkpath(Directory);
    return Directory;
}

[[nodiscard]] QString BaseDirectoryPath() {
    return EnsureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
}
}

[[nodiscard]] QString AudioDirectoryPath() {
    return EnsureDirectory(BaseDirectoryPath() + "/Audio");
}

[[nodiscard]] QString CrashLogFilePath() {
    return BaseDirectoryPath() + "/crash.log";
}

[[nodiscard]] QString DatabaseFilePath() {
    return BaseDirectoryPath() + "/memly.duckdb";
}
}
