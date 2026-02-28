#include "SupportData.hpp"

#include <filesystem>
#include <string>

static std::string EnsureDirectory(const std::string& Directory) {
    std::filesystem::create_directories(Directory);
    return Directory;
}

std::string TestApp::SupportData::BaseDirectoryPath() {
    return EnsureDirectory(std::string{ CMAKE_GENERATED_PROJECT_ROOT_DIR } +
                           "/test");
}

std::string TestApp::SupportData::DatabaseFilePath() {
    const std::string FilePath{ TestApp::SupportData::BaseDirectoryPath() +
                                "/test.duckdb" };
    std::error_code ErrorCode{};
    std::filesystem::remove(FilePath, ErrorCode);
    return FilePath;
}

std::string TestApp::SupportData::AudioDirectoryPath() {
    return EnsureDirectory(TestApp::SupportData::BaseDirectoryPath() +
                           "/Audio");
}
