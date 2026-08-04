/// \file
/// \brief Defines and initializes the process-wide runtime object graph.
///
/// \attention Keep static member definitions and `RuntimeContext::Initialize` construction ordered from lower-level dependencies to higher-level dependents.

#include "CompositionRoot/RuntimeContext.hpp"

#include <cassert>
#include <memory>
#include <string>

// #include "Layer/Application/Invalidation/LibraryInvalidationChannel.hpp"
// #include "Layer/Application/Invalidation/LibraryInvalidationCoordinator.hpp"
#include "Layer/Application/Service/Deck/DeckService.hpp"
// #include "Layer/Application/Service/ReviewSession/ReviewSessionListService.hpp"
// #include "Layer/Application/Service/ReviewSession/ReviewSessionService.hpp"
#include "Layer/Infrastructure/Persistence/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Persistence/Store/Deck/DeckSnapshotStore.hpp"
#include "Layer/Infrastructure/Persistence/Store/Deck/DeckStore.hpp"

// #include "Layer/Infrastructure/Persistence/Store/Library/LibraryClockStore.hpp"
// #include "Layer/Infrastructure/Persistence/Store/ReviewSession/ReviewSessionListStore.hpp"
// #include "Layer/Infrastructure/Persistence/Store/ReviewSession/ReviewSessionStore.hpp"

namespace CompositionRoot {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
std::unique_ptr<Layer::Infrastructure::Persistence::Database::DatabaseRuntime> RuntimeContext::s_DatabaseRuntime{};
// std::unique_ptr<Layer::Application::Invalidation::LibraryInvalidationChannel> RuntimeContext::s_LibraryInvalidationChannel{};
// std::unique_ptr<Layer::Infrastructure::Persistence::Store::Library::LibraryClockStore> RuntimeContext::s_LibraryClockStore{};
// std::unique_ptr<Layer::Application::Invalidation::LibraryInvalidationCoordinator> RuntimeContext::s_LibraryInvalidationCoordinator{};
std::unique_ptr<Layer::Infrastructure::Persistence::Store::Deck::DeckStore> RuntimeContext::s_DeckStore{};
std::unique_ptr<Layer::Infrastructure::Persistence::Store::Deck::DeckSnapshotStore> RuntimeContext::s_DeckSnapshotStore{};
// std::unique_ptr<Layer::Infrastructure::Persistence::Store::ReviewSession::ReviewSessionListStore> RuntimeContext::s_ReviewSessionListStore{};
// std::unique_ptr<Layer::Infrastructure::Persistence::Store::ReviewSession::ReviewSessionStore> RuntimeContext::s_ReviewSessionStore{};
std::unique_ptr<Layer::Application::Service::Deck::DeckService> RuntimeContext::s_DeckService{};
// std::unique_ptr<Layer::Application::Service::ReviewSession::ReviewSessionListService> RuntimeContext::s_ReviewSessionListService{};
// std::unique_ptr<Layer::Application::Service::ReviewSession::ReviewSessionService> RuntimeContext::s_ReviewSessionService{};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

void RuntimeContext::Initialize(const std::string& DatabaseFilePath) {
    assert(not DatabaseFilePath.empty());
    assert(s_DatabaseRuntime == nullptr);
    // assert(s_LibraryInvalidationChannel == nullptr);
    // assert(s_LibraryClockStore == nullptr);
    // assert(s_LibraryInvalidationCoordinator == nullptr);
    assert(s_DeckStore == nullptr);
    assert(s_DeckSnapshotStore == nullptr);
    // assert(s_ReviewSessionListStore == nullptr);
    // assert(s_ReviewSessionStore == nullptr);
    assert(s_DeckService == nullptr);
    // assert(s_ReviewSessionListService == nullptr);
    // assert(s_ReviewSessionService == nullptr);

    s_DatabaseRuntime = std::make_unique<Layer::Infrastructure::Persistence::Database::DatabaseRuntime>(DatabaseFilePath);
    // s_LibraryInvalidationChannel = std::make_unique<Layer::Application::Invalidation::LibraryInvalidationChannel>();
    // s_LibraryClockStore = std::make_unique<Layer::Infrastructure::Persistence::Store::Library::LibraryClockStore>(*s_DatabaseRuntime);
    // s_LibraryInvalidationCoordinator =
    //     std::make_unique<Layer::Application::Invalidation::LibraryInvalidationCoordinator>(*s_LibraryInvalidationChannel, *s_LibraryClockStore);
    s_DeckStore = std::make_unique<Layer::Infrastructure::Persistence::Store::Deck::DeckStore>(*s_DatabaseRuntime);
    s_DeckSnapshotStore = std::make_unique<Layer::Infrastructure::Persistence::Store::Deck::DeckSnapshotStore>(*s_DatabaseRuntime);
    // s_ReviewSessionListStore = std::make_unique<Layer::Infrastructure::Persistence::Store::ReviewSession::ReviewSessionListStore>(*s_DatabaseRuntime);
    // s_ReviewSessionStore = std::make_unique<Layer::Infrastructure::Persistence::Store::ReviewSession::ReviewSessionStore>(*s_DatabaseRuntime);
    s_DeckService = std::make_unique<Layer::Application::Service::Deck::DeckService>(*s_DeckStore, *s_DeckSnapshotStore);
    // s_ReviewSessionListService = std::make_unique<Layer::Application::Service::ReviewSession::ReviewSessionListService>(*s_ReviewSessionListStore);
    // s_ReviewSessionService =
    //     std::make_unique<Layer::Application::Service::ReviewSession::ReviewSessionService>(s_DatabaseRuntime->GetTransactionRunner(), *s_ReviewSessionStore);
}

// [[nodiscard]] auto RuntimeContext::GetRequiredLibraryInvalidationChannel() noexcept -> Layer::Application::Invalidation::LibraryInvalidationChannel& {
//     assert(s_LibraryInvalidationChannel not_eq nullptr);
//     return *s_LibraryInvalidationChannel;
// }

[[nodiscard]] auto RuntimeContext::GetRequiredDeckService() noexcept -> Layer::Application::Service::Deck::DeckService& {
    assert(s_DeckService not_eq nullptr);
    return *s_DeckService;
}

// [[nodiscard]] auto RuntimeContext::GetRequiredReviewSessionListService() noexcept -> Layer::Application::Service::ReviewSession::ReviewSessionListService& {
//     assert(s_ReviewSessionListService not_eq nullptr);
//     return *s_ReviewSessionListService;
// }

// [[nodiscard]] auto RuntimeContext::GetRequiredReviewSessionService() noexcept -> Layer::Application::Service::ReviewSession::ReviewSessionService& {
//     assert(s_ReviewSessionService not_eq nullptr);
//     return *s_ReviewSessionService;
// }

}
