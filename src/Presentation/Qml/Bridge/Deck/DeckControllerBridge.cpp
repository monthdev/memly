#include "Presentation/Qml/Bridge/Deck/DeckControllerBridge.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>

namespace Presentation::Qml::Bridge::Deck {
namespace {

[[nodiscard]] QString u_DeckControllerExpectedToQString(const std::expected<void, std::string_view>& DeckControllerExpected) noexcept {
    if (DeckControllerExpected.has_value()) {
        return QString{};
    }
    return QString::fromUtf8(DeckControllerExpected.error().data());
}

}

[[nodiscard]] QString DeckControllerBridge::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return u_DeckControllerExpectedToQString(m_DeckController.CreateRootDeck(DeckName.toStdString(), static_cast<std::uint8_t>(TargetLanguageCode)));
}

[[nodiscard]] QString DeckControllerBridge::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return u_DeckControllerExpectedToQString(m_DeckController.CreateChildDeck(DeckName.toStdString(), ParentDeckId.toStdString()));
}

[[nodiscard]] QString DeckControllerBridge::MoveDeck(const QString& DeckId, const QString& NewParentDeckId) noexcept {
    return u_DeckControllerExpectedToQString(
        m_DeckController.MoveDeck(DeckId.toStdString(), NewParentDeckId.isEmpty() ? std::nullopt : std::make_optional(NewParentDeckId.toStdString())));
}

[[nodiscard]] QString DeckControllerBridge::RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
    return u_DeckControllerExpectedToQString(m_DeckController.RenameDeck(DeckId.toStdString(), NewDeckName.toStdString()));
}

[[nodiscard]] QString DeckControllerBridge::DeleteDeck(const QString& DeckId) noexcept {
    return u_DeckControllerExpectedToQString(m_DeckController.DeleteDeck(DeckId.toStdString()));
}

}
