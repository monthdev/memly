#include "Support/AppStoragePath.hpp"

#include <QDir>
#include <QStandardPaths>

namespace Support::AppStoragePath {
namespace {
QString EnsureDirectory(const QString& Directory) {
    QDir().mkpath(Directory);
    return Directory;
}

QString BaseDirectoryPath() {
    return EnsureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
}
}

QString AudioDirectoryPath() {
    return EnsureDirectory(BaseDirectoryPath() + "/Audio");
}

QString CrashLogFilePath() {
    return BaseDirectoryPath() + "/crash.log";
}

QString DatabaseFilePath() {
    return BaseDirectoryPath() + "/memly.duckdb";
}
}
