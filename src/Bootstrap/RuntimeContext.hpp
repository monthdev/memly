/// \file
/// \brief Declares the process-wide runtime object graph.
///
/// \attention Keep RuntimeContext members ordered from lower-level dependencies to higher-level dependents.

#pragma once

#include <QString>
#include <memory>

namespace Application::Invalidation {
class LibraryInvalidationChannel;
class LibraryInvalidationCoordinator;
}

namespace Application::Service::Deck {
class DeckService;
class DeckTreeService;
}

namespace Application::Service::ReviewSession {
class ReviewSessionListService;
class ReviewSessionService;
}

namespace duckdb {
class Connection;
class DuckDB;
}

namespace Infrastructure::Sql {
class TransactionRunner;
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

    static Application::Invalidation::LibraryInvalidationChannel& GetRequiredLibraryInvalidationChannel() noexcept;
    static Application::Service::Deck::DeckService& GetRequiredDeckService() noexcept;
    static Application::Service::Deck::DeckTreeService& GetRequiredDeckTreeService() noexcept;
    static Application::Service::ReviewSession::ReviewSessionListService& GetRequiredReviewSessionListService() noexcept;
    static Application::Service::ReviewSession::ReviewSessionService& GetRequiredReviewSessionService() noexcept;

private:
    static std::unique_ptr<duckdb::DuckDB> s_Database;
    static std::unique_ptr<duckdb::Connection> s_DatabaseConnection;
    static std::unique_ptr<Infrastructure::Sql::TransactionRunner> s_TransactionRunner;
    static std::unique_ptr<Application::Invalidation::LibraryInvalidationChannel> s_LibraryInvalidationChannel;
    static std::unique_ptr<Infrastructure::Store::Library::LibraryClockStore> s_LibraryClockStore;
    static std::unique_ptr<Application::Invalidation::LibraryInvalidationCoordinator> s_LibraryInvalidationCoordinator;
    static std::unique_ptr<Infrastructure::Store::Deck::DeckStore> s_DeckStore;
    static std::unique_ptr<Infrastructure::Store::Deck::DeckTreeStore> s_DeckTreeStore;
    static std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionListStore> s_ReviewSessionListStore;
    static std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionStore> s_ReviewSessionStore;
    static std::unique_ptr<Application::Service::Deck::DeckService> s_DeckService;
    static std::unique_ptr<Application::Service::Deck::DeckTreeService> s_DeckTreeService;
    static std::unique_ptr<Application::Service::ReviewSession::ReviewSessionListService> s_ReviewSessionListService;
    static std::unique_ptr<Application::Service::ReviewSession::ReviewSessionService> s_ReviewSessionService;
};

}
