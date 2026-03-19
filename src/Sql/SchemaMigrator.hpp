#pragma once

#include <duckdb.hpp>

namespace Sql {

void RunMigrations(duckdb::Connection&);

}
