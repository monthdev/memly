// Temporarily disabled during deck page controller refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Presentation/Controller/DeckPageController.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <utility>
#include <vector>

#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Layer/Application/Service/Deck/DeckService.hpp"
#include "Support/Runtime/Exception/ExceptionBoundary.hpp"

namespace Layer::Presentation::Controller {

[[nodiscard]] auto DeckPageController::GetDeckForestModel() noexcept -> Model::DeckForestModel* {
    return &this->m_DeckForestModel;
}

void DeckPageController::RefreshDeckForestModel(const std::int64_t AsOfMillisecondsSinceEpoch) noexcept {
    Support::Runtime::Exception::TryCatchWrapper([&]() -> void {
        std::vector<Application::Domain::Deck::Index::DeckForestSnapshotNode> DeckForestSnapshotNodeVector{
            this->m_DeckService.ReadDeckForestSnapshotNodes(AsOfMillisecondsSinceEpoch)
        };
        this->m_DeckForestModel.ReplaceAll(std::move(DeckForestSnapshotNodeVector));
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::CreateRootDeck(const std::string& DeckName,
                                                                                  const std::uint8_t TargetLanguageCode) noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.CreateRootDeck(DeckName, TargetLanguageCode);
        return std::expected<void, const char*>{};
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.CreateChildDeck(DeckName, ParentDeckId);
        return std::expected<void, const char*>{};
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::MoveDeck(const std::string& DeckId,
                                                                            const std::optional<std::string>& NewParentDeckIdOptional) noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.MoveDeck(DeckId, NewParentDeckIdOptional);
        return std::expected<void, const char*>{};
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.RenameDeck(DeckId, NewDeckName);
        return std::expected<void, const char*>{};
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::DeleteDeck(const std::string& DeckId) noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.DeleteDeck(DeckId);
        return std::expected<void, const char*>{};
    });
}

}
#endif
