export module Memly.Bridge.DeckForestProxyModel;

// Temporarily disabled during deck forest proxy model refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qidentityproxymodel.h>
#include <qqmlintegration.h>
#include <qvariant.h>

import Memly.Model.DeckForestModel;

export namespace Memly::Bridge {

class DeckForestProxyModel final : public QIdentityProxyModel {
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit DeckForestProxyModel(Model::DeckForestModel&, QObject* = nullptr);

    explicit DeckForestProxyModel(const DeckForestProxyModel&) = delete;
    DeckForestProxyModel&
    operator=(const DeckForestProxyModel&) = delete;

    explicit DeckForestProxyModel(DeckForestProxyModel&&) = delete;
    DeckForestProxyModel&
    operator=(DeckForestProxyModel&&) = delete;

    ~DeckForestProxyModel() noexcept override = default;

    void
    sort(int, Qt::SortOrder = Qt::AscendingOrder) noexcept override;

    [[nodiscard]] QVariant
    headerData(
        int,
        Qt::Orientation,
        int = Qt::DisplayRole
    ) const noexcept override;

    [[nodiscard]] QHash<int, QByteArray>
    roleNames() const noexcept override;
};

}
#endif
