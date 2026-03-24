#pragma once

namespace duckdb {
class Connection;
}

namespace Sql {
void RunDatabaseBootstrap(duckdb::Connection&);
}
