#if 0
// Temporarily disabled during deck forest proxy model refactor.
#pragma once

#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qidentityproxymodel.h>
#include <qqmlintegration.h>
#include <qvariant.h>

#include "Presentation/Model/DeckForestModel.hpp"

namespace View::Bridge::ProxyModel {

class DeckForestProxyModel final : public QIdentityProxyModel {
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit DeckForestProxyModel(Presentation::Model::DeckForestModel& DeckForestModel, QObject* Parent = nullptr)
        : QIdentityProxyModel{ Parent } {
        setSourceModel(&DeckForestModel);
    }
    DeckForestProxyModel(const DeckForestProxyModel&) = delete;
    DeckForestProxyModel(DeckForestProxyModel&&) = delete;
    DeckForestProxyModel& operator=(const DeckForestProxyModel&) = delete;
    DeckForestProxyModel& operator=(DeckForestProxyModel&&) = delete;

    void sort(int, Qt::SortOrder = Qt::AscendingOrder) noexcept override;
    [[nodiscard]] QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const noexcept override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const noexcept override;
};

}
#endif
