#pragma once

#include <QString>
#include <memory>

namespace Store {
class DeckStore;
class DeckHierarchyStore;
}

namespace App::Bootstrap {

class DatabaseContext;

class AppContext final {
public:
    static void Initialize(const QString& DatabaseFilePath);
    static Store::DeckHierarchyStore& GetRequiredDeckHierarchyStore() noexcept;
    static Store::DeckStore& GetRequiredDeckStore() noexcept;

private:
    static std::unique_ptr<DatabaseContext> s_DatabaseContext;
    static std::unique_ptr<Store::DeckHierarchyStore> s_DeckHierarchyStore;
    static std::unique_ptr<Store::DeckStore> s_DeckStore;
};

}
