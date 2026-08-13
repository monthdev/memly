// Temporarily disabled during deck page controller refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Controller/DeckPageController.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <utility>
#include <vector>

#include "Memly/Domain/DeckForestSnapshotNode.hpp"
#include "Memly/Exception/ExceptionBoundary.hpp"
#include "Memly/Invalidation/LibraryInvalidationChannel.hpp"
#include "Memly/Service/DeckService.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Presentation::Controller {

DeckPageController::DeckPageController(Application::Invalidation::LibraryInvalidationChannel& LibraryInvalidationChannel,
                                       Application::Service::DeckService& DeckService,
                                       QObject* Parent)
    : QObject{ Parent }
    , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_DeckService{ DeckService }
    , m_DeckForestModel{ this } {
    LibraryInvalidationChannel.ConnectSnapshot(
        this, Application::Invalidation::LibraryInvalidationTargetEnum::DeckForestSnapshot, &DeckPageController::RefreshDeckForestModel);
}

[[nodiscard]] auto DeckPageController::GetDeckForestModel() noexcept -> Model::DeckForestModel* {
    return &this->m_DeckForestModel;
}

void DeckPageController::RefreshDeckForestModel(const std::int64_t AsOfMillisecondsSinceEpoch) noexcept {
    Support::Exception::TryCatchWrapper([&]() -> void {
        std::vector<Application::Domain::DeckForestSnapshotNode> DeckForestSnapshotNodeVector{
            this->m_DeckService.ReadDeckForestSnapshotNodes(AsOfMillisecondsSinceEpoch)
        };
        this->m_DeckForestModel.ReplaceAll(std::move(DeckForestSnapshotNodeVector));
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::CreateRootDeck(const std::string& DeckName,
                                                                                  const std::uint8_t TargetLanguageCode) noexcept {
    return Support::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.CreateRootDeck(DeckName, TargetLanguageCode);
        return std::expected<void, const char*>{};
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) noexcept {
    return Support::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.CreateChildDeck(DeckName, ParentDeckId);
        return std::expected<void, const char*>{};
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::MoveDeck(const std::string& DeckId,
                                                                            const std::optional<std::string>& NewParentDeckIdOptional) noexcept {
    return Support::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.MoveDeck(DeckId, NewParentDeckIdOptional);
        return std::expected<void, const char*>{};
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) noexcept {
    return Support::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.RenameDeck(DeckId, NewDeckName);
        return std::expected<void, const char*>{};
    });
}

[[nodiscard]] std::expected<void, const char*> DeckPageController::DeleteDeck(const std::string& DeckId) noexcept {
    return Support::Exception::TryCatchWrapper([&]() -> std::expected<void, const char*> {
        this->m_DeckService.DeleteDeck(DeckId);
        return std::expected<void, const char*>{};
    });
}

}
#endif
