/// \file
/// \brief Defines and initializes the process-wide runtime object graph.
///
/// \attention Keep static member definitions and `RuntimeContext::Initialize` construction ordered from lower-level dependencies to higher-level dependents.

#include "Bootstrap/RuntimeContext.hpp"

#include <QtGlobal>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Application/Invalidation/LibraryInvalidationCoordinator.hpp"
#include "Application/Service/Deck/DeckService.hpp"
#include "Application/Service/ReviewSession/ReviewSessionListService.hpp"
#include "Application/Service/ReviewSession/ReviewSessionService.hpp"
#include "Infrastructure/Database/DatabaseRuntime.hpp"
#include "Infrastructure/Store/Deck/DeckSnapshotStore.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"
#include "Infrastructure/Store/Library/LibraryClockStore.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

namespace Bootstrap {

std::unique_ptr<Infrastructure::Database::DatabaseRuntime> RuntimeContext::s_DatabaseRuntime{};
std::unique_ptr<Application::Invalidation::LibraryInvalidationChannel> RuntimeContext::s_LibraryInvalidationChannel{};
std::unique_ptr<Infrastructure::Store::Library::LibraryClockStore> RuntimeContext::s_LibraryClockStore{};
std::unique_ptr<Application::Invalidation::LibraryInvalidationCoordinator> RuntimeContext::s_LibraryInvalidationCoordinator{};
std::unique_ptr<Infrastructure::Store::Deck::DeckStore> RuntimeContext::s_DeckStore{};
std::unique_ptr<Infrastructure::Store::Deck::DeckSnapshotStore> RuntimeContext::s_DeckSnapshotStore{};
std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionListStore> RuntimeContext::s_ReviewSessionListStore{};
std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionStore> RuntimeContext::s_ReviewSessionStore{};
std::unique_ptr<Application::Service::Deck::DeckService> RuntimeContext::s_DeckService{};
std::unique_ptr<Application::Service::ReviewSession::ReviewSessionListService> RuntimeContext::s_ReviewSessionListService{};
std::unique_ptr<Application::Service::ReviewSession::ReviewSessionService> RuntimeContext::s_ReviewSessionService{};

void RuntimeContext::Initialize(const QString& DatabaseFilePath) {
    Q_ASSERT(not DatabaseFilePath.isEmpty());
    Q_ASSERT(s_DatabaseRuntime == nullptr);
    Q_ASSERT(s_LibraryInvalidationChannel == nullptr);
    Q_ASSERT(s_LibraryClockStore == nullptr);
    Q_ASSERT(s_LibraryInvalidationCoordinator == nullptr);
    Q_ASSERT(s_DeckStore == nullptr);
    Q_ASSERT(s_DeckSnapshotStore == nullptr);
    Q_ASSERT(s_ReviewSessionListStore == nullptr);
    Q_ASSERT(s_ReviewSessionStore == nullptr);
    Q_ASSERT(s_DeckService == nullptr);
    Q_ASSERT(s_ReviewSessionListService == nullptr);
    Q_ASSERT(s_ReviewSessionService == nullptr);

    s_DatabaseRuntime = std::make_unique<Infrastructure::Database::DatabaseRuntime>(DatabaseFilePath.toStdString());
    s_LibraryInvalidationChannel = std::make_unique<Application::Invalidation::LibraryInvalidationChannel>();
    s_LibraryClockStore = std::make_unique<Infrastructure::Store::Library::LibraryClockStore>(s_DatabaseRuntime->GetDatabaseConnection());
    s_LibraryInvalidationCoordinator =
        std::make_unique<Application::Invalidation::LibraryInvalidationCoordinator>(*s_LibraryInvalidationChannel, *s_LibraryClockStore);
    s_DeckStore = std::make_unique<Infrastructure::Store::Deck::DeckStore>(s_DatabaseRuntime->GetDatabaseConnection());
    s_DeckSnapshotStore = std::make_unique<Infrastructure::Store::Deck::DeckSnapshotStore>(s_DatabaseRuntime->GetDatabaseConnection());
    s_ReviewSessionListStore = std::make_unique<Infrastructure::Store::ReviewSession::ReviewSessionListStore>(s_DatabaseRuntime->GetDatabaseConnection());
    s_ReviewSessionStore = std::make_unique<Infrastructure::Store::ReviewSession::ReviewSessionStore>(s_DatabaseRuntime->GetDatabaseConnection());
    s_DeckService = std::make_unique<Application::Service::Deck::DeckService>(*s_DeckStore, *s_DeckSnapshotStore);
    s_ReviewSessionListService = std::make_unique<Application::Service::ReviewSession::ReviewSessionListService>(*s_ReviewSessionListStore);
    s_ReviewSessionService =
        std::make_unique<Application::Service::ReviewSession::ReviewSessionService>(s_DatabaseRuntime->GetTransactionRunner(), *s_ReviewSessionStore);
}

[[nodiscard]] Application::Invalidation::LibraryInvalidationChannel& RuntimeContext::GetRequiredLibraryInvalidationChannel() noexcept {
    Q_ASSERT(s_LibraryInvalidationChannel not_eq nullptr);
    return *s_LibraryInvalidationChannel;
}

[[nodiscard]] Application::Service::Deck::DeckService& RuntimeContext::GetRequiredDeckService() noexcept {
    Q_ASSERT(s_DeckService not_eq nullptr);
    return *s_DeckService;
}

[[nodiscard]] Application::Service::ReviewSession::ReviewSessionListService& RuntimeContext::GetRequiredReviewSessionListService() noexcept {
    Q_ASSERT(s_ReviewSessionListService not_eq nullptr);
    return *s_ReviewSessionListService;
}

[[nodiscard]] Application::Service::ReviewSession::ReviewSessionService& RuntimeContext::GetRequiredReviewSessionService() noexcept {
    Q_ASSERT(s_ReviewSessionService not_eq nullptr);
    return *s_ReviewSessionService;
}

}
