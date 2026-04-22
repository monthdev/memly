#include "App/Bootstrap/AppContext.hpp"

#include <QtGlobal>

#include "App/Bootstrap/DatabaseContext.hpp"
#include "Store/DeckStore.hpp"
#include "Store/DeckHierarchyStore.hpp"

namespace App {

std::unique_ptr<DatabaseContext> AppContext::s_DatabaseContext{};
std::unique_ptr<Store::DeckHierarchyStore> AppContext::s_DeckHierarchyStore{};
std::unique_ptr<Store::DeckStore> AppContext::s_DeckStore{};

void AppContext::Initialize(const QString& DatabaseFilePath) {
    Q_ASSERT(not DatabaseFilePath.isEmpty());
    Q_ASSERT(s_DatabaseContext == nullptr);
    Q_ASSERT(s_DeckHierarchyStore == nullptr);
    Q_ASSERT(s_DeckStore == nullptr);

    s_DatabaseContext = std::make_unique<DatabaseContext>(DatabaseFilePath);
    duckdb::Connection& DatabaseConnection{ s_DatabaseContext->GetConnection() };
    s_DeckHierarchyStore = std::make_unique<Store::DeckHierarchyStore>(DatabaseConnection);
    s_DeckStore = std::make_unique<Store::DeckStore>(DatabaseConnection);
}

Store::DeckHierarchyStore& AppContext::GetRequiredDeckHierarchyStore() noexcept {
    Q_ASSERT(s_DeckHierarchyStore != nullptr);
    return *s_DeckHierarchyStore;
}

Store::DeckStore& AppContext::GetRequiredDeckStore() noexcept {
    Q_ASSERT(s_DeckStore != nullptr);
    return *s_DeckStore;
}

}
