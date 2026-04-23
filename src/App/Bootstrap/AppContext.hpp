#pragma once

#include <QString>
#include <memory>

namespace Infrastructure::Store {
class DeckStore;
class DeckHierarchyStore;
}

namespace App::Bootstrap {

class DatabaseContext;

class AppContext final {
public:
    static void Initialize(const QString& DatabaseFilePath);
    static Infrastructure::Store::DeckHierarchyStore& GetRequiredDeckHierarchyStore() noexcept;
    static Infrastructure::Store::DeckStore& GetRequiredDeckStore() noexcept;

private:
    static std::unique_ptr<DatabaseContext> s_DatabaseContext;
    static std::unique_ptr<Infrastructure::Store::DeckHierarchyStore> s_DeckHierarchyStore;
    static std::unique_ptr<Infrastructure::Store::DeckStore> s_DeckStore;
};

}
