#include "Application/Service/Deck/DeckService.hpp"

#include <cstdint>
#include <expected>
#include <string>

#include "Application/Invalidation/LibraryInvalidationCoordinator.hpp"
#include "Domain/Deck/DeckConstraint.hpp"
#include "Infrastructure/Sql/TransactionRunner.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"

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

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::CreateRootDeck(const std::string& DeckName,
                                                                                                              const std::uint8_t TargetLanguageCode) {
    if (not Domain::Deck::IsDeckNameLengthValid(DeckName)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError };
    }
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateRootDeckExpected{ m_DeckStore.CreateRootDeck(DeckName,
                                                                                                                                     TargetLanguageCode) };
        if (not CreateRootDeckExpected.has_value()) {
            return std::unexpected{ CreateRootDeckExpected.error() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::CreateChildDeck(const std::string& DeckName,
                                                                                                               const std::string& ParentDeckId) {
    if (not Domain::Deck::IsDeckNameLengthValid(DeckName)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError };
    }
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateChildDeckExpected{ m_DeckStore.CreateChildDeck(DeckName,
                                                                                                                                       ParentDeckId) };
        if (not CreateChildDeckExpected.has_value()) {
            return std::unexpected{ CreateChildDeckExpected.error() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::MoveDeck(const std::string& DeckId,
                                                                                                        const std::optional<std::string>& NewParentDeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> MoveDeckExpected{ m_DeckStore.MoveDeck(DeckId, NewParentDeckId) };
        if (not MoveDeckExpected.has_value()) {
            return std::unexpected{ MoveDeckExpected.error() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::RenameDeck(const std::string& DeckId,
                                                                                                          const std::string& NewDeckName) {
    if (not Domain::Deck::IsDeckNameLengthValid(NewDeckName)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError };
    }
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RenameDeckExpected{ m_DeckStore.RenameDeck(DeckId, NewDeckName) };
        if (not RenameDeckExpected.has_value()) {
            return std::unexpected{ RenameDeckExpected.error() };
        }
        m_LibraryInvalidationCoordinator.Invalidate(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckService::DeleteDeck(const std::string& DeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> {
        const std::expected<void, Domain::Deck::RecoverableDeckIdErrorEnum> DeleteDeckExpected{ m_DeckStore.DeleteDeck(DeckId) };
        if (not DeleteDeckExpected.has_value()) {
            return std::unexpected{ u_RecoverableDeckIdErrorToMutationError(DeleteDeckExpected.error()) };
        }
        m_LibraryInvalidationCoordinator.InvalidateWithReschedule(Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot);
        return {};
    });
}

}
