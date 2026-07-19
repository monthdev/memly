/// \file
/// \brief Declares the process-wide runtime object graph.
///
/// \attention Keep `RuntimeContext` members ordered from lower-level dependencies to higher-level dependents.

#pragma once

#include <memory>
#include <string>

namespace Application::Invalidation {
class LibraryInvalidationChannel;
class LibraryInvalidationCoordinator;
}

namespace Application::Service::Deck {
class DeckService;
}

namespace Application::Service::ReviewSession {
class ReviewSessionListService;
class ReviewSessionService;
}

namespace Infrastructure::Database {
class DatabaseRuntime;
}

namespace Infrastructure::Store::Deck {
class DeckStore;
class DeckSnapshotStore;
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
private:
    static std::unique_ptr<Infrastructure::Database::DatabaseRuntime> s_DatabaseRuntime;
    static std::unique_ptr<Application::Invalidation::LibraryInvalidationChannel> s_LibraryInvalidationChannel;
    static std::unique_ptr<Infrastructure::Store::Library::LibraryClockStore> s_LibraryClockStore;
    static std::unique_ptr<Application::Invalidation::LibraryInvalidationCoordinator> s_LibraryInvalidationCoordinator;
    static std::unique_ptr<Infrastructure::Store::Deck::DeckStore> s_DeckStore;
    static std::unique_ptr<Infrastructure::Store::Deck::DeckSnapshotStore> s_DeckSnapshotStore;
    static std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionListStore> s_ReviewSessionListStore;
    static std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionStore> s_ReviewSessionStore;
    static std::unique_ptr<Application::Service::Deck::DeckService> s_DeckService;
    static std::unique_ptr<Application::Service::ReviewSession::ReviewSessionListService> s_ReviewSessionListService;
    static std::unique_ptr<Application::Service::ReviewSession::ReviewSessionService> s_ReviewSessionService;

public:
    explicit RuntimeContext() = delete;
    explicit RuntimeContext(const RuntimeContext&) = delete;
    explicit RuntimeContext(RuntimeContext&&) = delete;
    auto operator=(const RuntimeContext&) -> RuntimeContext& = delete;
    auto operator=(RuntimeContext&&) -> RuntimeContext& = delete;

    static void Initialize(const std::string&);

    [[nodiscard]] static auto GetRequiredLibraryInvalidationChannel() noexcept -> Application::Invalidation::LibraryInvalidationChannel&;
    [[nodiscard]] static auto GetRequiredDeckService() noexcept -> Application::Service::Deck::DeckService&;
    [[nodiscard]] static auto GetRequiredReviewSessionListService() noexcept -> Application::Service::ReviewSession::ReviewSessionListService&;
    [[nodiscard]] static auto GetRequiredReviewSessionService() noexcept -> Application::Service::ReviewSession::ReviewSessionService&;
};

}
