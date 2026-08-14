// Temporarily disabled during deck page controller bridge refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtypes.h>

#include "Memly/Bridge/DeckForestProxyModel.hpp"
#include "Memly/Controller/DeckPageController.hpp"
#include "Memly/Invalidation/LibraryInvalidationChannel.hpp"
#include "Memly/Service/DeckService.hpp"

namespace Memly::Bridge {

class DeckPageControllerBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(DeckForestProxyModel* deckForestModel READ GetDeckForestProxyModel CONSTANT)
    QML_NAMED_ELEMENT(DeckPageController)

private:
    Controller::DeckPageController m_DeckPageController;
    DeckForestProxyModel m_DeckForestProxyModel;

public:
    explicit DeckPageControllerBridge(Invalidation::LibraryInvalidationChannel&,
                                      Service::DeckService&,
                                      QObject* = nullptr);

    explicit DeckPageControllerBridge(const DeckPageControllerBridge&) = delete;
    auto operator=(const DeckPageControllerBridge&) -> DeckPageControllerBridge& = delete;

    explicit DeckPageControllerBridge(DeckPageControllerBridge&&) = delete;
    auto operator=(DeckPageControllerBridge&&) -> DeckPageControllerBridge& = delete;

    ~DeckPageControllerBridge() noexcept override = default;

    [[nodiscard]] DeckForestProxyModel* GetDeckForestProxyModel() noexcept;

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString RenameDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString DeleteDeck(const QString&) noexcept;
};

}
#endif
