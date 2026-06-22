#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Domain/Deck/DeckTreeSnapshotNode.hpp"
#include "Domain/Deck/RecoverableDeckError.hpp"

namespace Infrastructure::Database {
class TransactionRunner;
}

namespace Infrastructure::Store::Deck {
class DeckStore;
class DeckTreeSnapshotStore;
}

namespace Application::Service::Deck {

class DeckService final {
public:
    DeckService(Infrastructure::Database::TransactionRunner& TransactionRunner,
                Infrastructure::Store::Deck::DeckStore& DeckStore,
                Infrastructure::Store::Deck::DeckTreeSnapshotStore& DeckTreeSnapshotStore) noexcept
        : m_TransactionRunner{ TransactionRunner }
        , m_DeckStore{ DeckStore }
        , m_DeckTreeSnapshotStore{ DeckTreeSnapshotStore } {
    }

    ~DeckService() noexcept = default;
    DeckService(const DeckService&) = delete;
    DeckService(DeckService&&) = delete;
    DeckService& operator=(const DeckService&) = delete;
    DeckService& operator=(DeckService&&) = delete;

    [[nodiscard]] std::vector<Domain::Deck::DeckTreeSnapshotNode> ReadDeckTreeSnapshotNodes(std::int64_t);

    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateRootDeck(const std::string&, std::uint8_t);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateChildDeck(const std::string&, const std::string&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> MoveDeck(const std::string&, const std::optional<std::string>&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RenameDeck(const std::string&, const std::string&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeleteDeck(const std::string&);

private:
    Infrastructure::Database::TransactionRunner& m_TransactionRunner;
    Infrastructure::Store::Deck::DeckStore& m_DeckStore;
    Infrastructure::Store::Deck::DeckTreeSnapshotStore& m_DeckTreeSnapshotStore;
};

}
