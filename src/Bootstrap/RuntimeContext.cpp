#include "Bootstrap/RuntimeContext.hpp"

#include <duckdb.hpp>

#include <QtGlobal>

#include "Application/Coordinator/LibraryRefreshCoordinator.hpp"
#include "Infrastructure/Sql/DatabaseBootstrap.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"
#include "Infrastructure/Store/Deck/DeckTreeStore.hpp"
#include "Infrastructure/Store/Library/LibraryClockStore.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

namespace Bootstrap {

std::unique_ptr<duckdb::DuckDB> RuntimeContext::s_Database{};
std::unique_ptr<duckdb::Connection> RuntimeContext::s_DatabaseConnection{};
std::unique_ptr<Infrastructure::Store::Library::LibraryClockStore> RuntimeContext::s_LibraryClockStore{};
std::unique_ptr<Application::Coordinator::LibraryRefreshCoordinator> RuntimeContext::s_LibraryRefreshCoordinator{};
std::unique_ptr<Infrastructure::Store::Deck::DeckStore> RuntimeContext::s_DeckStore{};
std::unique_ptr<Infrastructure::Store::Deck::DeckTreeStore> RuntimeContext::s_DeckTreeStore{};
std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionListStore> RuntimeContext::s_ReviewSessionListStore{};
std::unique_ptr<Infrastructure::Store::ReviewSession::ReviewSessionStore> RuntimeContext::s_ReviewSessionStore{};

void RuntimeContext::Initialize(const QString& DatabaseFilePath) {
    Q_ASSERT(not DatabaseFilePath.isEmpty());
    Q_ASSERT(s_Database == nullptr);
    Q_ASSERT(s_DatabaseConnection == nullptr);
    Q_ASSERT(s_LibraryClockStore == nullptr);
    Q_ASSERT(s_LibraryRefreshCoordinator == nullptr);
    Q_ASSERT(s_DeckStore == nullptr);
    Q_ASSERT(s_DeckTreeStore == nullptr);
    Q_ASSERT(s_ReviewSessionListStore == nullptr);
    Q_ASSERT(s_ReviewSessionStore == nullptr);

    s_Database = std::make_unique<duckdb::DuckDB>(DatabaseFilePath.toStdString());
    s_DatabaseConnection = std::make_unique<duckdb::Connection>(*s_Database);
    Infrastructure::Sql::RunDatabaseBootstrap(*s_DatabaseConnection);
    s_LibraryClockStore = std::make_unique<Infrastructure::Store::Library::LibraryClockStore>(*s_DatabaseConnection);
    s_LibraryRefreshCoordinator = std::make_unique<Application::Coordinator::LibraryRefreshCoordinator>(*s_LibraryClockStore);
    s_DeckStore = std::make_unique<Infrastructure::Store::Deck::DeckStore>(*s_DatabaseConnection);
    s_DeckTreeStore = std::make_unique<Infrastructure::Store::Deck::DeckTreeStore>(*s_DatabaseConnection);
    s_ReviewSessionListStore = std::make_unique<Infrastructure::Store::ReviewSession::ReviewSessionListStore>(*s_DatabaseConnection);
    s_ReviewSessionStore = std::make_unique<Infrastructure::Store::ReviewSession::ReviewSessionStore>(*s_DatabaseConnection);
}

Application::Coordinator::LibraryRefreshCoordinator& RuntimeContext::GetRequiredLibraryRefreshCoordinator() noexcept {
    Q_ASSERT(s_LibraryRefreshCoordinator not_eq nullptr);
    return *s_LibraryRefreshCoordinator;
}

Infrastructure::Store::Deck::DeckStore& RuntimeContext::GetRequiredDeckStore() noexcept {
    Q_ASSERT(s_DeckStore not_eq nullptr);
    return *s_DeckStore;
}

Infrastructure::Store::Deck::DeckTreeStore& RuntimeContext::GetRequiredDeckTreeStore() noexcept {
    Q_ASSERT(s_DeckTreeStore not_eq nullptr);
    return *s_DeckTreeStore;
}

Infrastructure::Store::ReviewSession::ReviewSessionListStore& RuntimeContext::GetRequiredReviewSessionListStore() noexcept {
    Q_ASSERT(s_ReviewSessionListStore not_eq nullptr);
    return *s_ReviewSessionListStore;
}

Infrastructure::Store::ReviewSession::ReviewSessionStore& RuntimeContext::GetRequiredReviewSessionStore() noexcept {
    Q_ASSERT(s_ReviewSessionStore not_eq nullptr);
    return *s_ReviewSessionStore;
}

}
