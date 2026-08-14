// Temporarily disabled during deck page controller bridge refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Bridge/DeckPageControllerBridge.hpp"

#include <cstdint>
#include <expected>
#include <optional>

#include "Memly/Invalidation/LibraryInvalidationChannel.hpp"
#include "Memly/Service/DeckService.hpp"

namespace Memly::Bridge {

DeckPageControllerBridge::DeckPageControllerBridge(Invalidation::LibraryInvalidationChannel& LibraryInvalidationChannel,
                                                   Service::DeckService& DeckService,
                                                   QObject* Parent)
    : QObject{ Parent }
    , m_DeckPageController{ LibraryInvalidationChannel, DeckService }
    , m_DeckForestProxyModel{ *this->m_DeckPageController.GetDeckForestModel(), this } {
}

[[nodiscard]] DeckForestProxyModel* DeckPageControllerBridge::GetDeckForestProxyModel() noexcept {
    return &this->m_DeckForestProxyModel;
}

namespace {

[[nodiscard]] QString u_DeckPageControllerExpectedToQString(const std::expected<void, const char*>& DeckPageControllerExpected) noexcept {
    if (DeckPageControllerExpected.has_value()) {
        return QString{};
    }
    return QString::fromUtf8(DeckPageControllerExpected.error());
}

}

[[nodiscard]] QString DeckPageControllerBridge::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return u_DeckPageControllerExpectedToQString(
        this->m_DeckPageController.CreateRootDeck(DeckName.toStdString(), static_cast<std::uint8_t>(TargetLanguageCode)));
}

[[nodiscard]] QString DeckPageControllerBridge::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return u_DeckPageControllerExpectedToQString(this->m_DeckPageController.CreateChildDeck(DeckName.toStdString(), ParentDeckId.toStdString()));
}

[[nodiscard]] QString DeckPageControllerBridge::MoveDeck(const QString& DeckId, const QString& NewParentDeckId) noexcept {
    return u_DeckPageControllerExpectedToQString(
        this->m_DeckPageController.MoveDeck(DeckId.toStdString(), NewParentDeckId.isEmpty() ? std::nullopt : std::make_optional(NewParentDeckId.toStdString())));
}

[[nodiscard]] QString DeckPageControllerBridge::RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
    return u_DeckPageControllerExpectedToQString(this->m_DeckPageController.RenameDeck(DeckId.toStdString(), NewDeckName.toStdString()));
}

[[nodiscard]] QString DeckPageControllerBridge::DeleteDeck(const QString& DeckId) noexcept {
    return u_DeckPageControllerExpectedToQString(this->m_DeckPageController.DeleteDeck(DeckId.toStdString()));
}

}
#endif
