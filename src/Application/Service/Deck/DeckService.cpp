#include "Application/Service/Deck/DeckService.hpp"

#include "Application/Invalidation/LibraryInvalidationCoordinator.hpp"
#include "Infrastructure/Sql/TransactionRunner.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"

namespace Application::Service::Deck {

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::CreateRootDeck(const QString& DeckName,
                                                                                                              const quint8 TargetLanguageCode) {
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

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::CreateChildDeck(const QString& DeckName,
                                                                                                               const QString& ParentDeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.CreateChildDeck(DeckName, ParentDeckId) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ RecoverableDeckMutationError.value() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::MoveDeck(const QString& DeckId,
                                                                                                        const std::optional<QString>& NewParentDeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.MoveDeck(DeckId, NewParentDeckId) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ RecoverableDeckMutationError.value() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::RenameDeck(const QString& DeckId, const QString& NewDeckName) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.RenameDeck(DeckId, NewDeckName) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ RecoverableDeckMutationError.value() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::DeleteDeck(const QString& DeckId) {
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
