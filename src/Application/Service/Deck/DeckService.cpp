#include "Application/Service/Deck/DeckService.hpp"

#include "Infrastructure/Sql/TransactionRunner.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"

namespace {

[[nodiscard]] Application::Service::Deck::DeckService::DeckMutationErrorEnum
ToDeckMutationError(const Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum RecoverableDeckMutationError) noexcept {
    switch (RecoverableDeckMutationError) {
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::DeckNameLengthError:
        return Application::Service::Deck::DeckService::DeckMutationErrorEnum::DeckNameLengthError;
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::DuplicateSiblingDeckNameError:
        return Application::Service::Deck::DeckService::DeckMutationErrorEnum::DuplicateSiblingDeckNameError;
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::InvalidTargetLanguageCodeError:
        return Application::Service::Deck::DeckService::DeckMutationErrorEnum::InvalidTargetLanguageCodeError;
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError:
        return Application::Service::Deck::DeckService::DeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError;
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::DeckTreeCycleDetectionError:
        return Application::Service::Deck::DeckService::DeckMutationErrorEnum::DeckTreeCycleDetectionError;
    }
}

}

namespace Application::Service::Deck {

DeckService::DeckService(Infrastructure::Sql::TransactionRunner& TransactionRunner, Infrastructure::Store::Deck::DeckStore& DeckStore) noexcept
    : m_TransactionRunner{ TransactionRunner }
    , m_DeckStore{ DeckStore } {
}

[[nodiscard]] std::expected<void, DeckService::DeckMutationErrorEnum> DeckService::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, DeckMutationErrorEnum> {
        const std::optional<Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.CreateRootDeck(
            DeckName, TargetLanguageCode) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ ToDeckMutationError(RecoverableDeckMutationError.value()) };
        }
        return {};
    });
}

[[nodiscard]] std::expected<void, DeckService::DeckMutationErrorEnum> DeckService::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, DeckMutationErrorEnum> {
        const std::optional<Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.CreateChildDeck(
            DeckName, ParentDeckId) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ ToDeckMutationError(RecoverableDeckMutationError.value()) };
        }
        return {};
    });
}

[[nodiscard]] std::expected<void, DeckService::DeckMutationErrorEnum> DeckService::MoveDeck(const QString& DeckId,
                                                                                            const std::optional<QString>& NewParentDeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, DeckMutationErrorEnum> {
        const std::optional<Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.MoveDeck(
            DeckId, NewParentDeckId) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ ToDeckMutationError(RecoverableDeckMutationError.value()) };
        }
        return {};
    });
}

[[nodiscard]] std::expected<void, DeckService::DeckMutationErrorEnum> DeckService::RenameDeck(const QString& DeckId, const QString& NewDeckName) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, DeckMutationErrorEnum> {
        const std::optional<Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.RenameDeck(
            DeckId, NewDeckName) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ ToDeckMutationError(RecoverableDeckMutationError.value()) };
        }
        return {};
    });
}

[[nodiscard]] std::expected<void, DeckService::DeckMutationErrorEnum> DeckService::DeleteDeck(const QString& DeckId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, DeckMutationErrorEnum> {
        const std::optional<Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ m_DeckStore.DeleteDeck(
            DeckId) };
        if (RecoverableDeckMutationError.has_value()) {
            return std::unexpected{ ToDeckMutationError(RecoverableDeckMutationError.value()) };
        }
        return {};
    });
}

}
