#include "Presentation/Controller/DeckPageController.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include "Application/Service/Deck/DeckService.hpp"
#include "Application/Service/Deck/DeckTreeSnapshotService.hpp"
#include "Domain/Deck/DeckTreeSnapshotNode.hpp"
#include "Domain/Deck/RecoverableDeckError.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller {
namespace {

[[nodiscard]] constexpr std::string_view
u_RecoverableDeckMutationErrorToStringView(const Domain::Deck::RecoverableDeckMutationErrorEnum RecoverableDeckMutationError) noexcept {
    switch (RecoverableDeckMutationError) {
    case Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidDeckIdError: {
        return "Deck no longer exists";
    }
    case Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidParentDeckIdError: {
        return "Parent deck no longer exists";
    }
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
u_DeckServiceExpectedToDeckPageControllerExpected(const std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum>& DeckServiceExpected) noexcept {
    if (DeckServiceExpected.has_value()) {
        return {};
    }
    return std::unexpected{ u_RecoverableDeckMutationErrorToStringView(DeckServiceExpected.error()) };
}

}

void DeckPageController::RefreshDeckTreeModel(const std::int64_t AsOfMillisecondsSinceEpoch) noexcept {
    Runtime::TryCatchWrapper([&]() -> void {
        std::vector<Domain::Deck::DeckTreeSnapshotNode> DeckTreeNodeVector{ m_DeckTreeSnapshotService.ReadDeckTreeSnapshotNodes(AsOfMillisecondsSinceEpoch) };
        m_DeckTreeModel.ReplaceAll(std::move(DeckTreeNodeVector));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::CreateRootDeck(const std::string& DeckName,
                                                                                       const std::uint8_t TargetLanguageCode) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        return u_DeckServiceExpectedToDeckPageControllerExpected(m_DeckService.CreateRootDeck(DeckName, TargetLanguageCode));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        return u_DeckServiceExpectedToDeckPageControllerExpected(m_DeckService.CreateChildDeck(DeckName, ParentDeckId));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::MoveDeck(const std::string& DeckId,
                                                                                 const std::optional<std::string>& NewParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        return u_DeckServiceExpectedToDeckPageControllerExpected(m_DeckService.MoveDeck(DeckId, NewParentDeckId));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        return u_DeckServiceExpectedToDeckPageControllerExpected(m_DeckService.RenameDeck(DeckId, NewDeckName));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::DeleteDeck(const std::string& DeckId) noexcept {
    return Runtime::TryCatchWrapper(
        [&]() -> std::expected<void, std::string_view> { return u_DeckServiceExpectedToDeckPageControllerExpected(m_DeckService.DeleteDeck(DeckId)); });
}

}
