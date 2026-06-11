/// \file
/// \brief Defines and initializes the process-wide runtime object graph.
///
/// \attention Keep static member definitions and `RuntimeContext::Initialize` construction ordered from lower-level dependencies to higher-level dependents.

#include "Bootstrap/RuntimeContext.hpp"

#include <duckdb.hpp>

#include <QtGlobal>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Application/Invalidation/LibraryInvalidationCoordinator.hpp"
#include "Application/Service/Deck/DeckService.hpp"
#include "Application/Service/Deck/DeckTreeService.hpp"
#include "Application/Service/ReviewSession/ReviewSessionListService.hpp"
#include "Application/Service/ReviewSession/ReviewSessionService.hpp"
#include "Infrastructure/Sql/DatabaseBootstrap.hpp"
#include "Infrastructure/Sql/TransactionRunner.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"
#include "Infrastructure/Store/Deck/DeckTreeStore.hpp"
#include "Infrastructure/Store/Library/LibraryClockStore.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

namespace Bootstrap {

std::unique_ptr<duckdb::DuckDB> RuntimeContext::s_Database{};
std::unique_ptr<duckdb::Connection> RuntimeContext::s_DatabaseConnection{};
std::unique_ptr<Infrastructure::Sql::TransactionRunner> RuntimeContext::s_TransactionRunner{};
std::unique_ptr<Application::Invalidation::LibraryInvalidationChannel> RuntimeContext::s_LibraryInvalidationChannel{};
std::unique_ptr<Infrastructure::Store::Library::LibraryClockStore> RuntimeContext::s_LibraryClockStore{};
std::unique_ptr<Application::Invalidation::LibraryInvalidationCoordinator> RuntimeContext::s_LibraryInvalidationCoordinator{};
std::unique_ptr<Infrastructure::Store::Deck::DeckStore> RuntimeContext::s_DeckStore{};
std::unique_ptr<Infrastructure::Store::Deck::DeckTreeStore> RuntimeContext::s_DeckTreeStore{};
std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionListStore> RuntimeContext::s_ReviewSessionListStore{};
std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionStore> RuntimeContext::s_ReviewSessionStore{};
std::unique_ptr<Application::Service::Deck::DeckService> RuntimeContext::s_DeckService{};
std::unique_ptr<Application::Service::Deck::DeckTreeService> RuntimeContext::s_DeckTreeService{};
std::unique_ptr<Application::Service::ReviewSession::ReviewSessionListService> RuntimeContext::s_ReviewSessionListService{};
std::unique_ptr<Application::Service::ReviewSession::ReviewSessionService> RuntimeContext::s_ReviewSessionService{};

void RuntimeContext::Initialize(const QString& DatabaseFilePath) {
    Q_ASSERT(not DatabaseFilePath.isEmpty());
    Q_ASSERT(s_Database == nullptr);
    Q_ASSERT(s_DatabaseConnection == nullptr);
    Q_ASSERT(s_TransactionRunner == nullptr);
    Q_ASSERT(s_LibraryInvalidationChannel == nullptr);
    Q_ASSERT(s_LibraryClockStore == nullptr);
    Q_ASSERT(s_LibraryInvalidationCoordinator == nullptr);
    Q_ASSERT(s_DeckStore == nullptr);
    Q_ASSERT(s_DeckTreeStore == nullptr);
    Q_ASSERT(s_ReviewSessionListStore == nullptr);
    Q_ASSERT(s_ReviewSessionStore == nullptr);
    Q_ASSERT(s_DeckService == nullptr);
    Q_ASSERT(s_DeckTreeService == nullptr);
    Q_ASSERT(s_ReviewSessionListService == nullptr);
    Q_ASSERT(s_ReviewSessionService == nullptr);

    s_Database = std::make_unique<duckdb::DuckDB>(DatabaseFilePath.toStdString());
    s_DatabaseConnection = std::make_unique<duckdb::Connection>(*s_Database);
    Infrastructure::Sql::RunDatabaseBootstrap(*s_DatabaseConnection);
    s_TransactionRunner = std::make_unique<Infrastructure::Sql::TransactionRunner>(*s_DatabaseConnection);
    s_LibraryInvalidationChannel = std::make_unique<Application::Invalidation::LibraryInvalidationChannel>();
    s_LibraryClockStore = std::make_unique<Infrastructure::Store::Library::LibraryClockStore>(*s_DatabaseConnection);
    s_LibraryInvalidationCoordinator =
        std::make_unique<Application::Invalidation::LibraryInvalidationCoordinator>(*s_LibraryInvalidationChannel, *s_LibraryClockStore);
    s_DeckStore = std::make_unique<Infrastructure::Store::Deck::DeckStore>(*s_DatabaseConnection);
    s_DeckTreeStore = std::make_unique<Infrastructure::Store::Deck::DeckTreeStore>(*s_DatabaseConnection);
    s_ReviewSessionListStore = std::make_unique<Infrastructure::Store::ReviewSession::ReviewSessionListStore>(*s_DatabaseConnection);
    s_ReviewSessionStore = std::make_unique<Infrastructure::Store::ReviewSession::ReviewSessionStore>(*s_DatabaseConnection);
    s_DeckService = std::make_unique<Application::Service::Deck::DeckService>(*s_TransactionRunner, *s_LibraryInvalidationCoordinator, *s_DeckStore);
    s_DeckTreeService = std::make_unique<Application::Service::Deck::DeckTreeService>(*s_DeckTreeStore);
    s_ReviewSessionListService = std::make_unique<Application::Service::ReviewSession::ReviewSessionListService>(*s_ReviewSessionListStore);
    s_ReviewSessionService = std::make_unique<Application::Service::ReviewSession::ReviewSessionService>(*s_TransactionRunner, *s_ReviewSessionStore);
}

[[nodiscard]] Application::Invalidation::LibraryInvalidationChannel& RuntimeContext::GetRequiredLibraryInvalidationChannel() noexcept {
    Q_ASSERT(s_LibraryInvalidationChannel not_eq nullptr);
    return *s_LibraryInvalidationChannel;
}

[[nodiscard]] Application::Service::Deck::DeckService& RuntimeContext::GetRequiredDeckService() noexcept {
    Q_ASSERT(s_DeckService not_eq nullptr);
    return *s_DeckService;
}

[[nodiscard]] Application::Service::Deck::DeckTreeService& RuntimeContext::GetRequiredDeckTreeService() noexcept {
    Q_ASSERT(s_DeckTreeService not_eq nullptr);
    return *s_DeckTreeService;
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
