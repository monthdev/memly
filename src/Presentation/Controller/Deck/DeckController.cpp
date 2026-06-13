#include "Presentation/Controller/Deck/DeckController.hpp"

#include <expected>

#include "Application/Service/Deck/DeckService.hpp"
#include "Domain/Deck/RecoverableDeckMutationError.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller::Deck {
namespace {

[[nodiscard]] QString RecoverableDeckMutationErrorToQString(const Domain::Deck::RecoverableDeckMutationErrorEnum RecoverableDeckMutationError) {
    switch (RecoverableDeckMutationError) {
    case Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError: {
        return QString{ "Deck name length exceeds character limit" };
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::DuplicateSiblingDeckNameError: {
        return QString{ "Deck name already exists" };
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidTargetLanguageCodeError: {
        return QString{ "Target language code is invalid" };
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError: {
        return QString{ "Deck target language does not match parent deck" };
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::DeckTreeCycleDetectionError: {
        return QString{ "Deck cannot be moved into itself or one of its own sub decks" };
    }
    }
}

}

[[nodiscard]] std::optional<QString> DeckController::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.CreateRootDeck(
            DeckName, TargetLanguageCode) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToQString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckController::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.CreateChildDeck(DeckName,
                                                                                                                                               ParentDeckId) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToQString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckController::MoveDeck(const QString& DeckId, const std::optional<QString>& NewParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.MoveDeck(DeckId,
                                                                                                                                        NewParentDeckId) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToQString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckController::RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.RenameDeck(DeckId,
                                                                                                                                          NewDeckName) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToQString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckController::DeleteDeck(const QString& DeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.DeleteDeck(DeckId) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToQString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

}
