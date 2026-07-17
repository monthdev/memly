#if 0
// Temporarily disabled during deck page controller bridge refactor.
#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtypes.h>

#include "Bootstrap/RuntimeContext.hpp"
#include "Presentation/Controller/DeckPageController.hpp"
#include "View/Bridge/ProxyModel/DeckForestProxyModel.hpp"

namespace View::Bridge::Controller {

class DeckPageControllerBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(View::Bridge::ProxyModel::DeckForestProxyModel* deckForestModel READ GetDeckForestProxyModel CONSTANT)
    QML_NAMED_ELEMENT(DeckPageController)

public:
    explicit DeckPageControllerBridge(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckPageController{ Bootstrap::RuntimeContext::GetRequiredLibraryInvalidationChannel(), Bootstrap::RuntimeContext::GetRequiredDeckService() }
        , m_DeckForestProxyModel{ *m_DeckPageController.GetDeckForestModel(), this } {
    }
    DeckPageControllerBridge(const DeckPageControllerBridge&) = delete;
    DeckPageControllerBridge(DeckPageControllerBridge&&) = delete;
    DeckPageControllerBridge& operator=(const DeckPageControllerBridge&) = delete;
    DeckPageControllerBridge& operator=(DeckPageControllerBridge&&) = delete;

    [[nodiscard]] View::Bridge::ProxyModel::DeckForestProxyModel* GetDeckForestProxyModel() noexcept;

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString RenameDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString DeleteDeck(const QString&) noexcept;

private:
    Presentation::Controller::DeckPageController m_DeckPageController;
    View::Bridge::ProxyModel::DeckForestProxyModel m_DeckForestProxyModel;
};

}
#endif
