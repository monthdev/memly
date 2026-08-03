#pragma once

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace duckdb {
class Connection;
}

namespace Layer::Infrastructure::Database {

class DatabaseMigrator final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::Connection& m_DatabaseConnection;

public:
    explicit DatabaseMigrator(duckdb::Connection& DatabaseConnection) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseConnection{ DatabaseConnection } {
    }

    void ApplyMigrations();
};

}
