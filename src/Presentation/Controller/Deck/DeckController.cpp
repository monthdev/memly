#include "Presentation/Controller/Deck/DeckController.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>

#include "Application/Service/Deck/DeckService.hpp"
#include "Domain/Deck/RecoverableDeckMutationError.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller::Deck {
namespace {

[[nodiscard]] constexpr std::string_view
u_RecoverableDeckMutationErrorToStringView(const Domain::Deck::RecoverableDeckMutationErrorEnum RecoverableDeckMutationError) noexcept {
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

[[nodiscard]] constexpr std::expected<void, std::string_view>
u_DeckServiceExpectedToDeckControllerExpected(const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum>& DeckServiceExpected) noexcept {
    if (DeckServiceExpected.has_value()) {
        return {};
    }
    return std::unexpected{ u_RecoverableDeckMutationErrorToStringView(DeckServiceExpected.error()) };
}

}

[[nodiscard]] std::expected<void, std::string_view> DeckController::CreateRootDeck(const std::string& DeckName,
                                                                                   const std::uint8_t TargetLanguageCode) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        return u_DeckServiceExpectedToDeckControllerExpected(m_DeckService.CreateRootDeck(DeckName, TargetLanguageCode));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckController::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        return u_DeckServiceExpectedToDeckControllerExpected(m_DeckService.CreateChildDeck(DeckName, ParentDeckId));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckController::MoveDeck(const std::string& DeckId,
                                                                             const std::optional<std::string>& NewParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        return u_DeckServiceExpectedToDeckControllerExpected(m_DeckService.MoveDeck(DeckId, NewParentDeckId));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckController::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        return u_DeckServiceExpectedToDeckControllerExpected(m_DeckService.RenameDeck(DeckId, NewDeckName));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckController::DeleteDeck(const std::string& DeckId) noexcept {
    return Runtime::TryCatchWrapper(
        [&]() -> std::expected<void, std::string_view> { return u_DeckServiceExpectedToDeckControllerExpected(m_DeckService.DeleteDeck(DeckId)); });
}

}
