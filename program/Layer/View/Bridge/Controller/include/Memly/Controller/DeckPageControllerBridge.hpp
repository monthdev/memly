// Temporarily disabled during deck page controller bridge refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtypes.h>

#include "Memly/Controller/DeckPageController.hpp"
#include "Memly/ProxyModel/DeckForestProxyModel.hpp"

namespace Layer::Application::Invalidation {
class LibraryInvalidationChannel;
}

namespace Layer::Application::Service {
class DeckService;
}

namespace Layer::View::Bridge::Controller {

class DeckPageControllerBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(ProxyModel::DeckForestProxyModel* deckForestModel READ GetDeckForestProxyModel CONSTANT)
    QML_NAMED_ELEMENT(DeckPageController)

private:
    Presentation::Controller::DeckPageController m_DeckPageController;
    ProxyModel::DeckForestProxyModel m_DeckForestProxyModel;

public:
    explicit DeckPageControllerBridge(Application::Invalidation::LibraryInvalidationChannel&,
                                      Application::Service::DeckService&,
                                      QObject* = nullptr);

    explicit DeckPageControllerBridge(const DeckPageControllerBridge&) = delete;
    auto operator=(const DeckPageControllerBridge&) -> DeckPageControllerBridge& = delete;

    explicit DeckPageControllerBridge(DeckPageControllerBridge&&) = delete;
    auto operator=(DeckPageControllerBridge&&) -> DeckPageControllerBridge& = delete;

    ~DeckPageControllerBridge() noexcept override = default;

    [[nodiscard]] ProxyModel::DeckForestProxyModel* GetDeckForestProxyModel() noexcept;

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString RenameDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString DeleteDeck(const QString&) noexcept;
};

}
#endif
