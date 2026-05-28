#pragma once

#include <QtTypes>
#include <memory>
#include <optional>

namespace duckdb {
class Connection;
class PreparedStatement;
}

namespace Infrastructure::Store::Library {

class LibraryClockStore final {
public:
    explicit LibraryClockStore(duckdb::Connection&);
    ~LibraryClockStore();

    [[nodiscard]] std::optional<qint64> ReadNextLibraryRefreshAtMillisecondsSinceEpoch(const qint64);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadNextLibraryRefreshAtMillisecondsSinceEpochPreparedStatement;
};

}
