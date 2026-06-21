#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QByteArray>
#include <QHash>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QVariant>

#include "Presentation/Model/DeckTreeModel.hpp"

namespace View::Bridge::Model {

class DeckTreeModelBridge final : public QIdentityProxyModel {
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit DeckTreeModelBridge(Presentation::Model::DeckTreeModel& PresentationDeckTreeModel, QObject* Parent = nullptr)
        : QIdentityProxyModel{ Parent } {
        setSourceModel(&PresentationDeckTreeModel);
    }

    ~DeckTreeModelBridge() noexcept override = default;
    DeckTreeModelBridge(const DeckTreeModelBridge&) = delete;
    DeckTreeModelBridge(DeckTreeModelBridge&&) = delete;
    DeckTreeModelBridge& operator=(const DeckTreeModelBridge&) = delete;
    DeckTreeModelBridge& operator=(DeckTreeModelBridge&&) = delete;

    void sort(int, Qt::SortOrder = Qt::AscendingOrder) noexcept override;
    [[nodiscard]] QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const noexcept override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const noexcept override;
};

}
