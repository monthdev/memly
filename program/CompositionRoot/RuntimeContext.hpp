/// \file
/// \brief Declares the process-wide runtime object graph.
///
/// \attention Keep `RuntimeContext` members ordered from lower-level dependencies to higher-level dependents.

#pragma once

#include <memory>
#include <string>

#include "Support/SpecialMemberPolicy/NonInstantiableMixin.hpp"

// namespace Layer::Application::Invalidation {
// class LibraryInvalidationChannel;
// class LibraryInvalidationCoordinator;
// }

namespace Layer::Application::Service::Deck {
class DeckService;
}

// namespace Layer::Application::Service::ReviewSession {
// class ReviewSessionListService;
// class ReviewSessionService;
// }

namespace Layer::Infrastructure::Persistence::Database {
class DatabaseRuntime;
}

namespace Layer::Infrastructure::Persistence::Store::Deck {
class DeckStore;
class DeckSnapshotStore;
}

// namespace Layer::Infrastructure::Persistence::Store::Library {
// class LibraryClockStore;
// }

// namespace Layer::Infrastructure::Persistence::Store::ReviewSession {
// class ReviewSessionListStore;
// class ReviewSessionStore;
// }

namespace CompositionRoot {

class RuntimeContext final : private Support::SpecialMemberPolicy::NonInstantiableMixin {
private:
    static std::unique_ptr<Layer::Infrastructure::Persistence::Database::DatabaseRuntime> s_DatabaseRuntime;
    // static std::unique_ptr<Layer::Application::Invalidation::LibraryInvalidationChannel> s_LibraryInvalidationChannel;
    // static std::unique_ptr<Layer::Infrastructure::Persistence::Store::Library::LibraryClockStore> s_LibraryClockStore;
    // static std::unique_ptr<Layer::Application::Invalidation::LibraryInvalidationCoordinator> s_LibraryInvalidationCoordinator;
    static std::unique_ptr<Layer::Infrastructure::Persistence::Store::Deck::DeckStore> s_DeckStore;
    static std::unique_ptr<Layer::Infrastructure::Persistence::Store::Deck::DeckSnapshotStore> s_DeckSnapshotStore;
    // static std::unique_ptr<Layer::Infrastructure::Persistence::Store::ReviewSession::ReviewSessionListStore> s_ReviewSessionListStore;
    // static std::unique_ptr<Layer::Infrastructure::Persistence::Store::ReviewSession::ReviewSessionStore> s_ReviewSessionStore;
    static std::unique_ptr<Layer::Application::Service::Deck::DeckService> s_DeckService;
    // static std::unique_ptr<Layer::Application::Service::ReviewSession::ReviewSessionListService> s_ReviewSessionListService;
    // static std::unique_ptr<Layer::Application::Service::ReviewSession::ReviewSessionService> s_ReviewSessionService;

public:
    explicit RuntimeContext() = delete;

    static void Initialize(const std::string&);

    // [[nodiscard]] static auto GetRequiredLibraryInvalidationChannel() noexcept -> Layer::Application::Invalidation::LibraryInvalidationChannel&;
    [[nodiscard]] static auto GetRequiredDeckService() noexcept -> Layer::Application::Service::Deck::DeckService&;
    // [[nodiscard]] static auto GetRequiredReviewSessionListService() noexcept -> Layer::Application::Service::ReviewSession::ReviewSessionListService&;
    // [[nodiscard]] static auto GetRequiredReviewSessionService() noexcept -> Layer::Application::Service::ReviewSession::ReviewSessionService&;
};

}
