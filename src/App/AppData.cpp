#include "App/AppData.hpp"

#include <QDir>
#include <QStandardPaths>

namespace App {
static QString EnsureDirectory(const QString& Directory) {
    QDir().mkpath(Directory);
    return Directory;
}

static QString BaseDirectoryPath() {
    return EnsureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
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
