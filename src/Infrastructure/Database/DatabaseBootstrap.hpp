#pragma once

namespace duckdb {
class Connection;
}

namespace Infrastructure::Database {
void RunDatabaseBootstrap(duckdb::Connection&);
}
