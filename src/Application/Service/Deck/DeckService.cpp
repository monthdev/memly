#include "Application/Service/Deck/DeckService.hpp"

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

#include "Domain/Deck/DeckConstraint.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"
#include "Infrastructure/Store/Deck/DeckTreeSnapshotStore.hpp"

namespace Application::Service::Deck {
namespace {

[[nodiscard]] constexpr Domain::Deck::RecoverableDeckMutationErrorEnum
u_RecoverableDeckIdErrorToMutationError(const Domain::Deck::RecoverableDeckIdErrorEnum RecoverableDeckIdError) noexcept {
    switch (RecoverableDeckIdError) {
    case Domain::Deck::RecoverableDeckIdErrorEnum::InvalidDeckIdError: {
        return Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidDeckIdError;
    }
    case Domain::Deck::RecoverableDeckIdErrorEnum::InvalidParentDeckIdError: {
        return Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidParentDeckIdError;
    }
    }
}

}

[[nodiscard]] std::vector<Domain::Deck::DeckTreeSnapshotNode> DeckService::ReadDeckTreeSnapshotNodes(const std::int64_t AsOfMillisecondsSinceEpoch) {
    return m_DeckTreeSnapshotStore.ReadDeckTreeSnapshotNodes(AsOfMillisecondsSinceEpoch);
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::CreateRootDeck(const std::string& DeckName,
                                                                                                              const std::uint8_t TargetLanguageCode) {
    if (not Domain::Deck::IsDeckNameLengthValid(DeckName)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError };
    }
    return m_DeckStore.CreateRootDeck(DeckName, TargetLanguageCode);
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::CreateChildDeck(const std::string& DeckName,
                                                                                                               const std::string& ParentDeckId) {
    if (not Domain::Deck::IsDeckNameLengthValid(DeckName)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError };
    }
    return m_DeckStore.CreateChildDeck(DeckName, ParentDeckId);
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::MoveDeck(const std::string& DeckId,
                                                                                                        const std::optional<std::string>& NewParentDeckId) {
    return m_DeckStore.MoveDeck(DeckId, NewParentDeckId);
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::RenameDeck(const std::string& DeckId,
                                                                                                          const std::string& NewDeckName) {
    if (not Domain::Deck::IsDeckNameLengthValid(NewDeckName)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError };
    }
    return m_DeckStore.RenameDeck(DeckId, NewDeckName);
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::DeleteDeck(const std::string& DeckId) {
    const std::expected<void, Domain::Deck::RecoverableDeckIdErrorEnum> DeleteDeckExpected{ m_DeckStore.DeleteDeck(DeckId) };
    if (not DeleteDeckExpected.has_value()) {
        return std::unexpected{ u_RecoverableDeckIdErrorToMutationError(DeleteDeckExpected.error()) };
    }
    return {};
}

}
