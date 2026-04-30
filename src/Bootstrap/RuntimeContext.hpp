#pragma once

#include <QString>
#include <memory>

namespace Coordinator {
class LibraryRefreshCoordinator;
}

namespace duckdb {
class Connection;
class DuckDB;
}

namespace Infrastructure::Store {
class DeckStore;
class DeckTreeStore;
class LibraryClockStore;
}

namespace Bootstrap {

class RuntimeContext final {
public:
    static void Initialize(const QString&);

    static Coordinator::LibraryRefreshCoordinator& GetRequiredLibraryRefreshCoordinator() noexcept;
    static Infrastructure::Store::DeckStore& GetRequiredDeckStore() noexcept;
    static Infrastructure::Store::DeckTreeStore& GetRequiredDeckTreeStore() noexcept;

private:
    static std::unique_ptr<duckdb::DuckDB> s_Database;
    static std::unique_ptr<duckdb::Connection> s_DatabaseConnection;
    static std::unique_ptr<Infrastructure::Store::LibraryClockStore> s_LibraryClockStore;
    static std::unique_ptr<Coordinator::LibraryRefreshCoordinator> s_LibraryRefreshCoordinator;
    static std::unique_ptr<Infrastructure::Store::DeckStore> s_DeckStore;
    static std::unique_ptr<Infrastructure::Store::DeckTreeStore> s_DeckTreeStore;
};

}
