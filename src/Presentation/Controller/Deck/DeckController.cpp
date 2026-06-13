#include "Presentation/Controller/Deck/DeckController.hpp"

#include <cstdint>
#include <expected>
#include <string>

#include "Application/Service/Deck/DeckService.hpp"
#include "Domain/Deck/RecoverableDeckMutationError.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller::Deck {
namespace {

[[nodiscard]] std::string RecoverableDeckMutationErrorToString(const Domain::Deck::RecoverableDeckMutationErrorEnum RecoverableDeckMutationError) {
    switch (RecoverableDeckMutationError) {
    case Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError: {
        return "Deck name length exceeds character limit";
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::DuplicateSiblingDeckNameError: {
        return "Deck name already exists";
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidTargetLanguageCodeError: {
        return "Target language code is invalid";
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError: {
        return "Deck target language does not match parent deck";
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::DeckTreeCycleDetectionError: {
        return "Deck cannot be moved into itself or one of its own sub decks";
    }
    }
}

}

[[nodiscard]] std::optional<std::string> DeckController::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<std::string> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.CreateRootDeck(
            DeckName, TargetLanguageCode) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<std::string> DeckController::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<std::string> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.CreateChildDeck(DeckName,
                                                                                                                                               ParentDeckId) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<std::string> DeckController::MoveDeck(const std::string& DeckId, const std::optional<std::string>& NewParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<std::string> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.MoveDeck(DeckId,
                                                                                                                                        NewParentDeckId) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<std::string> DeckController::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<std::string> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.RenameDeck(DeckId,
                                                                                                                                          NewDeckName) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<std::string> DeckController::DeleteDeck(const std::string& DeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<std::string> {
        const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationResult{ m_DeckService.DeleteDeck(DeckId) };
        if (not RecoverableDeckMutationResult.has_value()) {
            return RecoverableDeckMutationErrorToString(RecoverableDeckMutationResult.error());
        }
        return std::nullopt;
    });
}

}
