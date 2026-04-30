#include "Bootstrap/RuntimeContext.hpp"

#include <duckdb.hpp>

#include <QtGlobal>

#include "Coordinator/LibraryRefreshCoordinator.hpp"
#include "Infrastructure/Sql/DatabaseBootstrap.hpp"
#include "Infrastructure/Store/DeckStore.hpp"
#include "Infrastructure/Store/DeckTreeStore.hpp"
#include "Infrastructure/Store/LibraryClockStore.hpp"

namespace Bootstrap {

std::unique_ptr<duckdb::DuckDB> RuntimeContext::s_Database{};
std::unique_ptr<duckdb::Connection> RuntimeContext::s_DatabaseConnection{};
std::unique_ptr<Infrastructure::Store::LibraryClockStore> RuntimeContext::s_LibraryClockStore{};
std::unique_ptr<Coordinator::LibraryRefreshCoordinator> RuntimeContext::s_LibraryRefreshCoordinator{};
std::unique_ptr<Infrastructure::Store::DeckStore> RuntimeContext::s_DeckStore{};
std::unique_ptr<Infrastructure::Store::DeckTreeStore> RuntimeContext::s_DeckTreeStore{};

void RuntimeContext::Initialize(const QString& DatabaseFilePath) {
    Q_ASSERT(not DatabaseFilePath.isEmpty());
    Q_ASSERT(s_Database == nullptr);
    Q_ASSERT(s_DatabaseConnection == nullptr);
    Q_ASSERT(s_LibraryClockStore == nullptr);
    Q_ASSERT(s_LibraryRefreshCoordinator == nullptr);
    Q_ASSERT(s_DeckStore == nullptr);
    Q_ASSERT(s_DeckTreeStore == nullptr);

    s_Database = std::make_unique<duckdb::DuckDB>(DatabaseFilePath.toStdString());
    s_DatabaseConnection = std::make_unique<duckdb::Connection>(*s_Database);
    Infrastructure::Sql::RunDatabaseBootstrap(*s_DatabaseConnection);
    s_LibraryClockStore = std::make_unique<Infrastructure::Store::LibraryClockStore>(*s_DatabaseConnection);
    s_LibraryRefreshCoordinator = std::make_unique<Coordinator::LibraryRefreshCoordinator>(*s_LibraryClockStore);
    s_DeckStore = std::make_unique<Infrastructure::Store::DeckStore>(*s_DatabaseConnection);
    s_DeckTreeStore = std::make_unique<Infrastructure::Store::DeckTreeStore>(*s_DatabaseConnection);
}

Coordinator::LibraryRefreshCoordinator& RuntimeContext::GetRequiredLibraryRefreshCoordinator() noexcept {
    Q_ASSERT(s_LibraryRefreshCoordinator not_eq nullptr);
    return *s_LibraryRefreshCoordinator;
}

Infrastructure::Store::DeckStore& RuntimeContext::GetRequiredDeckStore() noexcept {
    Q_ASSERT(s_DeckStore not_eq nullptr);
    return *s_DeckStore;
}

Infrastructure::Store::DeckTreeStore& RuntimeContext::GetRequiredDeckTreeStore() noexcept {
    Q_ASSERT(s_DeckTreeStore not_eq nullptr);
    return *s_DeckTreeStore;
}

}
