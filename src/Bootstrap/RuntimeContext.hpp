#pragma once

#include <QString>
#include <memory>

namespace Application::Coordinator {
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
class ReviewSessionListStore;
class ReviewSessionStore;
}

namespace Bootstrap {

class RuntimeContext final {
public:
    static void Initialize(const QString&);

    static Application::Coordinator::LibraryRefreshCoordinator& GetRequiredLibraryRefreshCoordinator() noexcept;
    static Infrastructure::Store::DeckStore& GetRequiredDeckStore() noexcept;
    static Infrastructure::Store::DeckTreeStore& GetRequiredDeckTreeStore() noexcept;
    static Infrastructure::Store::ReviewSessionListStore& GetRequiredReviewSessionListStore() noexcept;
    static Infrastructure::Store::ReviewSessionStore& GetRequiredReviewSessionStore() noexcept;

private:
    static std::unique_ptr<duckdb::DuckDB> s_Database;
    static std::unique_ptr<duckdb::Connection> s_DatabaseConnection;
    static std::unique_ptr<Infrastructure::Store::LibraryClockStore> s_LibraryClockStore;
    static std::unique_ptr<Application::Coordinator::LibraryRefreshCoordinator> s_LibraryRefreshCoordinator;
    static std::unique_ptr<Infrastructure::Store::DeckStore> s_DeckStore;
    static std::unique_ptr<Infrastructure::Store::DeckTreeStore> s_DeckTreeStore;
    static std::unique_ptr<Infrastructure::Store::ReviewSessionListStore> s_ReviewSessionListStore;
    static std::unique_ptr<Infrastructure::Store::ReviewSessionStore> s_ReviewSessionStore;
};

}
