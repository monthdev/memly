#include <gtest/gtest.h>

#include <fstream>

#include "Qt/App/SqlResource.hpp"
#include "Utility/Unrecoverable.hpp"

static const std::string ProjectRootPath{ CMAKE_GENERATED_PROJECT_ROOT_DIR };
static const std::string SqlDirectoryPath{ ProjectRootPath + "/src/Sql" };

static std::string FileContentsToString(const std::string& FilePath) {
    std::ifstream File(FilePath, std::ios::binary);
    if (!File) {
        Unrecoverable::Throw();
    }
    std::ostringstream Buffer{};
    Buffer << File.rdbuf();
    return Buffer.str();
}

TEST(SqlResource, InitializeSchemaSql) {
    EXPECT_EQ(
        Qt::App::SqlResource::InitializeSchemaSql(),
        FileContentsToString(SqlDirectoryPath + "/Migrations/Schema.sql"));
}
