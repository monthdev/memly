#pragma once

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace duckdb {
class Connection;
}

namespace Layer::Infrastructure::Database {

class DatabaseSeeder final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::Connection& m_DatabaseConnection;

public:
    explicit DatabaseSeeder(duckdb::Connection& DatabaseConnection) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseConnection{ DatabaseConnection } {
    }

    void SeedTableDefaults();
};

}
