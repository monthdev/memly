#if 0
// Temporarily disabled during deck page controller refactor.
#include "Presentation/Controller/DeckPageController.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include "Application/Domain/Deck/Data/DeckTreeSnapshotNode.hpp"
#include "Application/Service/Deck/DeckService.hpp"
#include "Support/Runtime/ExceptionBoundary.hpp"

namespace Presentation::Controller {

void DeckPageController::RefreshDeckTreeModel(const std::int64_t AsOfMillisecondsSinceEpoch) noexcept {
    Support::Runtime::TryCatchWrapper([&]() -> void {
        std::vector<Application::Domain::Deck::Data::DeckTreeSnapshotNode> DeckTreeNodeVector{
            m_DeckService.ReadDeckTreeSnapshotNodes(AsOfMillisecondsSinceEpoch)
        };
        m_DeckTreeModel.ReplaceAll(std::move(DeckTreeNodeVector));
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::CreateRootDeck(const std::string& DeckName,
                                                                                       const std::uint8_t TargetLanguageCode) noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        m_DeckService.CreateRootDeck(DeckName, TargetLanguageCode);
        return {};
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        m_DeckService.CreateChildDeck(DeckName, ParentDeckId);
        return {};
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::MoveDeck(const std::string& DeckId,
                                                                                 const std::optional<std::string>& NewParentDeckId) noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        m_DeckService.MoveDeck(DeckId, NewParentDeckId);
        return {};
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        m_DeckService.RenameDeck(DeckId, NewDeckName);
        return {};
    });
}

[[nodiscard]] std::expected<void, std::string_view> DeckPageController::DeleteDeck(const std::string& DeckId) noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> std::expected<void, std::string_view> {
        m_DeckService.DeleteDeck(DeckId);
        return {};
    });
}

}
#endif
