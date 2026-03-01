#include <gtest/gtest.h>

#include <QDir>

#include "Qt/App/SupportData.hpp"

TEST(SupportDataMacOS, BaseDirectoryPath) {
    EXPECT_EQ(Qt::App::SupportData::BaseDirectoryPath(),
              QDir::homePath().toStdString() + "/Library/Application Support");
}
