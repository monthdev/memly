#include "Application/Service/Deck/DeckService.hpp"

#include <cstdint>
#include <string>

#include "Application/Invalidation/LibraryInvalidationCoordinator.hpp"
#include "Infrastructure/Sql/TransactionRunner.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"

namespace Application::Service::Deck {

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::CreateRootDeck(const std::string& DeckName,
                                                                                                              const std::uint8_t TargetLanguageCode) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.CreateRootDeck(DeckName,
                                                                                                                                     TargetLanguageCode) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ RecoverableDeckMutationError.value() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::CreateChildDeck(const std::string& DeckName,
                                                                                                               const std::string& ParentDeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.CreateChildDeck(DeckName, ParentDeckId) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ RecoverableDeckMutationError.value() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::MoveDeck(const std::string& DeckId,
                                                                                                        const std::optional<std::string>& NewParentDeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.MoveDeck(DeckId, NewParentDeckId) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ RecoverableDeckMutationError.value() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::RenameDeck(const std::string& DeckId,
                                                                                                          const std::string& NewDeckName) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.RenameDeck(DeckId, NewDeckName) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ RecoverableDeckMutationError.value() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::DeleteDeck(const std::string& DeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.DeleteDeck(DeckId) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ RecoverableDeckMutationError.value() };
        }
        m_LibraryInvalidationCoordinator.InvalidateWithReschedule(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

}
