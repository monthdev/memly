#pragma once

#include <QtTypes>
#include <optional>

namespace duckdb {
class Connection;
}

namespace Infrastructure::Store {

class LibraryClockStore final {
public:
    explicit LibraryClockStore(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    [[nodiscard]] std::optional<qint64> ReadNextLibraryRefreshAtMillisecondsSinceEpoch(const qint64);

private:
    duckdb::Connection& m_DatabaseConnection;
};

}
