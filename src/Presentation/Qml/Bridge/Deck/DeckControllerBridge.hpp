#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>
#include <QString>
#include <QtTypes>
#include <cstdint>
#include <optional>
#include <string>

#include "Bootstrap/RuntimeContext.hpp"
#include "Presentation/Controller/Deck/DeckController.hpp"

namespace Presentation::Qml::Bridge::Deck {

class DeckControllerBridge : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(DeckController)

public:
    explicit DeckControllerBridge(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckController{ Bootstrap::RuntimeContext::GetRequiredDeckService() } {
    }

    ~DeckControllerBridge() noexcept override = default;
    DeckControllerBridge(const DeckControllerBridge&) = delete;
    DeckControllerBridge(DeckControllerBridge&&) = delete;
    DeckControllerBridge& operator=(const DeckControllerBridge&) = delete;
    DeckControllerBridge& operator=(DeckControllerBridge&&) = delete;

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
        const std::optional<std::string> RecoverableDeckMutationError{ m_DeckController.CreateRootDeck(DeckName.toStdString(),
                                                                                                       static_cast<std::uint8_t>(TargetLanguageCode)) };
        if (not RecoverableDeckMutationError.has_value()) {
            return QString{};
        }
        return QString::fromStdString(RecoverableDeckMutationError.value());
    }

    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
        const std::optional<std::string> RecoverableDeckMutationError{ m_DeckController.CreateChildDeck(DeckName.toStdString(), ParentDeckId.toStdString()) };
        if (not RecoverableDeckMutationError.has_value()) {
            return QString{};
        }
        return QString::fromStdString(RecoverableDeckMutationError.value());
    }

    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString& DeckId, const QString& NewParentDeckId = QString{}) noexcept {
        const std::optional<std::string> RecoverableDeckMutationError{ m_DeckController.MoveDeck(
            DeckId.toStdString(), NewParentDeckId.isEmpty() ? std::nullopt : std::make_optional(NewParentDeckId.toStdString())) };
        if (not RecoverableDeckMutationError.has_value()) {
            return QString{};
        }
        return QString::fromStdString(RecoverableDeckMutationError.value());
    }

    [[nodiscard]] Q_INVOKABLE QString RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
        const std::optional<std::string> RecoverableDeckMutationError{ m_DeckController.RenameDeck(DeckId.toStdString(), NewDeckName.toStdString()) };
        if (not RecoverableDeckMutationError.has_value()) {
            return QString{};
        }
        return QString::fromStdString(RecoverableDeckMutationError.value());
    }

    [[nodiscard]] Q_INVOKABLE QString DeleteDeck(const QString& DeckId) noexcept {
        const std::optional<std::string> RecoverableDeckMutationError{ m_DeckController.DeleteDeck(DeckId.toStdString()) };
        if (not RecoverableDeckMutationError.has_value()) {
            return QString{};
        }
        return QString::fromStdString(RecoverableDeckMutationError.value());
    }

private:
    Presentation::Controller::Deck::DeckController m_DeckController;
};

}
