#pragma once

#include <string>
#include <string_view>

namespace Infrastructure::Sql {

[[nodiscard]] std::string ReadSqlResourceBytes(const std::string_view);
}
