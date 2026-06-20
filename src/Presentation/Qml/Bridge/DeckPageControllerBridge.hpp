#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>
#include <QString>
#include <QtTypes>

#include "Bootstrap/RuntimeContext.hpp"
#include "Presentation/Controller/DeckPageController.hpp"

namespace Presentation::Qml::Bridge {

class DeckPageControllerBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::DeckTreeModel* deckTreeModel READ GetDeckTreeModel CONSTANT)
    QML_NAMED_ELEMENT(DeckPageController)

public:
    explicit DeckPageControllerBridge(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckPageController{ Bootstrap::RuntimeContext::GetRequiredLibraryInvalidationChannel(),
                                Bootstrap::RuntimeContext::GetRequiredDeckService(),
                                Bootstrap::RuntimeContext::GetRequiredDeckTreeSnapshotService() } {
    }

    ~DeckPageControllerBridge() noexcept override = default;
    DeckPageControllerBridge(const DeckPageControllerBridge&) = delete;
    DeckPageControllerBridge(DeckPageControllerBridge&&) = delete;
    DeckPageControllerBridge& operator=(const DeckPageControllerBridge&) = delete;
    DeckPageControllerBridge& operator=(DeckPageControllerBridge&&) = delete;

    [[nodiscard]] Presentation::Model::DeckTreeModel* GetDeckTreeModel() noexcept {
        return m_DeckPageController.GetDeckTreeModel();
    }

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString RenameDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString DeleteDeck(const QString&) noexcept;

private:
    Presentation::Controller::DeckPageController m_DeckPageController;
};

}
