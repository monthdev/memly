#pragma once

#include <QString>
#include <QtTypes>
#include <expected>
#include <optional>

#include "Domain/Deck/RecoverableDeckMutationError.hpp"

namespace Infrastructure::Sql {
class TransactionRunner;
}

namespace Infrastructure::Store::Deck {
class DeckStore;
}

namespace Application::Invalidation {
class LibraryInvalidationCoordinator;
}

namespace Application::Service::Deck {

class DeckService final {
public:
    DeckService(Infrastructure::Sql::TransactionRunner& TransactionRunner,
                Application::Invalidation::LibraryInvalidationCoordinator& LibraryInvalidationCoordinator,
                Infrastructure::Store::Deck::DeckStore& DeckStore) noexcept
        : m_TransactionRunner{ TransactionRunner }
        , m_LibraryInvalidationCoordinator{ LibraryInvalidationCoordinator }
        , m_DeckStore{ DeckStore } {
    }

    ~DeckService() noexcept = default;
    DeckService(const DeckService&) = delete;
    DeckService(DeckService&&) = delete;
    DeckService& operator=(const DeckService&) = delete;
    DeckService& operator=(DeckService&&) = delete;

    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateRootDeck(const QString&, quint8);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateChildDeck(const QString&, const QString&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> MoveDeck(const QString&, const std::optional<QString>&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RenameDeck(const QString&, const QString&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeleteDeck(const QString&);

private:
    Infrastructure::Sql::TransactionRunner& m_TransactionRunner;
    Application::Invalidation::LibraryInvalidationCoordinator& m_LibraryInvalidationCoordinator;
    Infrastructure::Store::Deck::DeckStore& m_DeckStore;
};

}
