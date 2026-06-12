#include "Presentation/Controller/Deck/DeckController.hpp"

#include <expected>

#include "Application/Service/Deck/DeckService.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller::Deck {
namespace {

[[nodiscard]] QString DeckMutationErrorToQString(const Application::Service::Deck::DeckService::DeckMutationErrorEnum DeckMutationError) {
    switch (DeckMutationError) {
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::DeckNameLengthError: {
        return QString{ "Deck name length exceeds character limit" };
    }
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::DuplicateSiblingDeckNameError: {
        return QString{ "Deck name already exists" };
    }
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::InvalidTargetLanguageCodeError: {
        return QString{ "Target language code is invalid" };
    }
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError: {
        return QString{ "Deck target language does not match parent deck" };
    }
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::DeckTreeCycleDetectionError: {
        return QString{ "Deck cannot be moved into itself or one of its own sub decks" };
    }
    }
}

}

[[nodiscard]] std::optional<QString> DeckController::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.CreateRootDeck(
            DeckName, TargetLanguageCode) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckController::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.CreateChildDeck(
            DeckName, ParentDeckId) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckController::MoveDeck(const QString& DeckId, const std::optional<QString>& NewParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.MoveDeck(DeckId,
                                                                                                                                             NewParentDeckId) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckController::RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.RenameDeck(DeckId,
                                                                                                                                               NewDeckName) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckController::DeleteDeck(const QString& DeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.DeleteDeck(DeckId) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        return std::nullopt;
    });
}

}
