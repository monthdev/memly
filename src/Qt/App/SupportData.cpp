#include "SupportData.hpp"

#include <QDir>
#include <QStandardPaths>

static std::string EnsureDirectory(const std::string& Directory) {
    QDir().mkpath(Directory.c_str());
    return Directory;
}

std::string App::SupportData::BaseDirectoryPath() {
    return EnsureDirectory(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
            .toStdString());
}

std::string App::SupportData::DatabaseFilePath() {
    return App::SupportData::BaseDirectoryPath() + "/memly.duckdb";
}

std::string App::SupportData::AudioDirectoryPath() {
    return EnsureDirectory(App::SupportData::BaseDirectoryPath() + "/Audio");
}
