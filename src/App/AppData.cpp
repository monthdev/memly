#include "App/AppData.hpp"

#include <QDir>
#include <QStandardPaths>

namespace App {
static std::string EnsureDirectory(const std::string& Directory) {
    QDir().mkpath(Directory.c_str());
    return Directory;
}

std::string BaseDirectoryPath() {
    return EnsureDirectory(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
            .toStdString());
}

std::string AudioDirectoryPath() {
    return EnsureDirectory(BaseDirectoryPath() + "/Audio");
}

std::string DatabaseFilePath() {
    return BaseDirectoryPath() + "/memly.duckdb";
}
}
