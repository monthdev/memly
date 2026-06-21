#include "View/Bridge/Controller/DeckPageControllerBridge.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>

namespace View::Bridge::Controller {
namespace {

[[nodiscard]] QString u_DeckPageControllerExpectedToQString(const std::expected<void, std::string_view>& DeckPageControllerExpected) noexcept {
    if (DeckPageControllerExpected.has_value()) {
        return QString{};
    }
    return QString::fromUtf8(DeckPageControllerExpected.error().data());
}

}

[[nodiscard]] View::Bridge::ProxyModel::DeckTreeProxyModel* DeckPageControllerBridge::GetDeckTreeProxyModel() noexcept {
    return &m_DeckTreeProxyModel;
}

[[nodiscard]] QString DeckPageControllerBridge::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return u_DeckPageControllerExpectedToQString(m_DeckPageController.CreateRootDeck(DeckName.toStdString(), static_cast<std::uint8_t>(TargetLanguageCode)));
}

[[nodiscard]] QString DeckPageControllerBridge::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return u_DeckPageControllerExpectedToQString(m_DeckPageController.CreateChildDeck(DeckName.toStdString(), ParentDeckId.toStdString()));
}

[[nodiscard]] QString DeckPageControllerBridge::MoveDeck(const QString& DeckId, const QString& NewParentDeckId) noexcept {
    return u_DeckPageControllerExpectedToQString(
        m_DeckPageController.MoveDeck(DeckId.toStdString(), NewParentDeckId.isEmpty() ? std::nullopt : std::make_optional(NewParentDeckId.toStdString())));
}

[[nodiscard]] QString DeckPageControllerBridge::RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
    return u_DeckPageControllerExpectedToQString(m_DeckPageController.RenameDeck(DeckId.toStdString(), NewDeckName.toStdString()));
}

[[nodiscard]] QString DeckPageControllerBridge::DeleteDeck(const QString& DeckId) noexcept {
    return u_DeckPageControllerExpectedToQString(m_DeckPageController.DeleteDeck(DeckId.toStdString()));
}

}
