#include <gtest/gtest.h>

#include "Qt/App/QmlResource.hpp"

TEST(QmlResource, MainWindow) {
    EXPECT_EQ(Qt::App::QmlResource::MainWindow(), "qrc:/Memly/MainWindow.qml");
}
