#pragma once

#include <string>

namespace Sql {
std::string InitialSchemaSql();

std::string CreateDeckSql();

std::string ReadDeckTableSql();

std::string UpdateDeckSql();

std::string DeleteDeckSql();

}
