#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] std::string ReadSqlResourceBytes(const char* SqlResourcePath);
}
