#include "Support/Runtime/AppStoragePath.hpp"

#include <QDir>
#include <QStandardPaths>

namespace Support::Runtime {
namespace {
[[nodiscard]] QString u_EnsureDirectory(const QString& Directory) {
    QDir().mkpath(Directory);
    return Directory;
}

[[nodiscard]] QString u_BaseDirectoryPath() {
    return u_EnsureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
}
}

[[nodiscard]] QString AudioDirectoryPath() {
    return u_EnsureDirectory(u_BaseDirectoryPath() + "/Audio");
}

[[nodiscard]] QString CrashLogFilePath() {
    return u_BaseDirectoryPath() + "/crash.log";
}

[[nodiscard]] QString DatabaseFilePath() {
    return u_BaseDirectoryPath() + "/memly.duckdb";
}
}
