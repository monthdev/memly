#include <gtest/gtest.h>

#include <QDir>

#include "Qt/App/SupportData.hpp"

static const std::string EXPECTED_BASE_DIRECTORY_PATH{
    QDir::homePath().toStdString() + "/Library/Application Support/Memly"
};

TEST(SupportDataMacOS, BaseDirectoryPath) {
    EXPECT_EQ(Qt::App::SupportData::BaseDirectoryPath(),
              EXPECTED_BASE_DIRECTORY_PATH);
}

TEST(SupportDataMacOS, AudioDirectoryPath) {
    EXPECT_EQ(Qt::App::SupportData::AudioDirectoryPath(),
              EXPECTED_BASE_DIRECTORY_PATH + "/Audio");
}

TEST(SupportDataMacOS, DatabaseFilePath) {
    EXPECT_EQ(Qt::App::SupportData::DatabaseFilePath(),
              EXPECTED_BASE_DIRECTORY_PATH + "/memly.duckdb");
}
