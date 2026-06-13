#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>

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

    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateRootDeck(const std::string&, std::uint8_t);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateChildDeck(const std::string&, const std::string&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> MoveDeck(const std::string&, const std::optional<std::string>&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RenameDeck(const std::string&, const std::string&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeleteDeck(const std::string&);

private:
    Infrastructure::Sql::TransactionRunner& m_TransactionRunner;
    Application::Invalidation::LibraryInvalidationCoordinator& m_LibraryInvalidationCoordinator;
    Infrastructure::Store::Deck::DeckStore& m_DeckStore;
};

}
