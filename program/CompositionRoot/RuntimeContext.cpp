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
#include "Layer/Infrastructure/DuckDb/Database/DatabaseMigrator.hpp"
#include "Layer/Infrastructure/DuckDb/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/Deck/DeckRepository.hpp"

// #include "Layer/Infrastructure/DuckDb/Repository/Library/LibraryRepository.hpp"
// #include "Layer/Infrastructure/DuckDb/Repository/ReviewSession/ReviewSessionRepository.hpp"

namespace CompositionRoot {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
std::unique_ptr<Layer::Infrastructure::DuckDb::Database::DatabaseRuntime> RuntimeContext::s_DatabaseRuntime{};
// std::unique_ptr<Layer::Application::Invalidation::LibraryInvalidationChannel> RuntimeContext::s_LibraryInvalidationChannel{};
// std::unique_ptr<Layer::Infrastructure::DuckDb::Repository::Library::LibraryRepository> RuntimeContext::s_LibraryRepository{};
// std::unique_ptr<Layer::Application::Invalidation::LibraryInvalidationCoordinator> RuntimeContext::s_LibraryInvalidationCoordinator{};
std::unique_ptr<Layer::Infrastructure::DuckDb::Repository::Deck::DeckRepository> RuntimeContext::s_DeckRepository{};
// std::unique_ptr<Layer::Infrastructure::DuckDb::Repository::ReviewSession::ReviewSessionRepository> RuntimeContext::s_ReviewSessionRepository{};
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
    // assert(s_LibraryRepository == nullptr);
    // assert(s_LibraryInvalidationCoordinator == nullptr);
    assert(s_DeckRepository == nullptr);
    // assert(s_ReviewSessionRepository == nullptr);
    assert(s_DeckService == nullptr);
    // assert(s_ReviewSessionListService == nullptr);
    // assert(s_ReviewSessionService == nullptr);

    // Direct construction preserves guaranteed copy elision for the nonmovable returned runtime.
    // NOLINTNEXTLINE(modernize-make-unique)
    s_DatabaseRuntime = std::unique_ptr<Layer::Infrastructure::DuckDb::Database::DatabaseRuntime>{ new Layer::Infrastructure::DuckDb::Database::DatabaseRuntime{
        Layer::Infrastructure::DuckDb::Database::DatabaseMigrator{ DatabaseFilePath }.ApplyMigrations() } };
    // s_LibraryInvalidationChannel = std::make_unique<Layer::Application::Invalidation::LibraryInvalidationChannel>();
    // s_LibraryRepository = std::make_unique<Layer::Infrastructure::DuckDb::Repository::Library::LibraryRepository>(*s_DatabaseRuntime);
    // s_LibraryInvalidationCoordinator =
    //     std::make_unique<Layer::Application::Invalidation::LibraryInvalidationCoordinator>(*s_LibraryInvalidationChannel, *s_LibraryRepository);
    s_DeckRepository = std::make_unique<Layer::Infrastructure::DuckDb::Repository::Deck::DeckRepository>(*s_DatabaseRuntime);
    // s_ReviewSessionRepository =
    // std::make_unique<Layer::Infrastructure::DuckDb::Repository::ReviewSession::ReviewSessionRepository>(*s_DatabaseRuntime);
    s_DeckService = std::make_unique<Layer::Application::Service::Deck::DeckService>(*s_DeckRepository);
    // s_ReviewSessionListService = std::make_unique<Layer::Application::Service::ReviewSession::ReviewSessionListService>(*s_ReviewSessionRepository);
    // Reintroduce ReviewSessionService after the application transaction boundary is defined.
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
