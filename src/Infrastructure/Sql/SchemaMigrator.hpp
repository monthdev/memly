#pragma once

namespace duckdb {
class Connection;
}

namespace Infrastructure::Sql {
void RunDatabaseBootstrap(duckdb::Connection&);
}
