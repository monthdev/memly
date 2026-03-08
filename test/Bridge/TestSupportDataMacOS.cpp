#include <gtest/gtest.h>

#include <QDir>

#include "Bridge/SupportData.hpp"

static const std::string EXPECTED_BASE_DIRECTORY_PATH{
    QDir::homePath().toStdString() + "/Library/Application Support/Memly"
};

TEST(SupportDataMacOS, BaseDirectoryPath) {
    EXPECT_EQ(BaseDirectoryPath(), EXPECTED_BASE_DIRECTORY_PATH);
}

TEST(SupportDataMacOS, AudioDirectoryPath) {
    EXPECT_EQ(AudioDirectoryPath(), EXPECTED_BASE_DIRECTORY_PATH + "/Audio");
}

TEST(SupportDataMacOS, DatabaseFilePath) {
    EXPECT_EQ(DatabaseFilePath(),
              EXPECTED_BASE_DIRECTORY_PATH + "/memly.duckdb");
}
