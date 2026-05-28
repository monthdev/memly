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

namespace Infrastructure::Store::Deck {
class DeckStore;
class DeckTreeStore;
}

namespace Infrastructure::Store::Library {
class LibraryClockStore;
}

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionListStore;
class ReviewSessionStore;
}

namespace Bootstrap {

class RuntimeContext final {
public:
    static void Initialize(const QString&);

    static Application::Coordinator::LibraryRefreshCoordinator& GetRequiredLibraryRefreshCoordinator() noexcept;
    static Infrastructure::Store::Deck::DeckStore& GetRequiredDeckStore() noexcept;
    static Infrastructure::Store::Deck::DeckTreeStore& GetRequiredDeckTreeStore() noexcept;
    static Infrastructure::Store::ReviewSession::ReviewSessionListStore& GetRequiredReviewSessionListStore() noexcept;
    static Infrastructure::Store::ReviewSession::ReviewSessionStore& GetRequiredReviewSessionStore() noexcept;

private:
    static std::unique_ptr<duckdb::DuckDB> s_Database;
    static std::unique_ptr<duckdb::Connection> s_DatabaseConnection;
    static std::unique_ptr<Infrastructure::Store::Library::LibraryClockStore> s_LibraryClockStore;
    static std::unique_ptr<Application::Coordinator::LibraryRefreshCoordinator> s_LibraryRefreshCoordinator;
    static std::unique_ptr<Infrastructure::Store::Deck::DeckStore> s_DeckStore;
    static std::unique_ptr<Infrastructure::Store::Deck::DeckTreeStore> s_DeckTreeStore;
    static std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionListStore> s_ReviewSessionListStore;
    static std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionStore> s_ReviewSessionStore;
};

}
