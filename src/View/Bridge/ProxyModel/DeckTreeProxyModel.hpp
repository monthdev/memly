#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QByteArray>
#include <QHash>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QVariant>

#include "Presentation/Model/DeckTreeModel.hpp"

namespace View::Bridge::ProxyModel {

class DeckTreeProxyModel final : public QIdentityProxyModel {
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit DeckTreeProxyModel(Presentation::Model::DeckTreeModel& DeckTreeModel, QObject* Parent = nullptr)
        : QIdentityProxyModel{ Parent } {
        setSourceModel(&DeckTreeModel);
    }

    ~DeckTreeProxyModel() noexcept override = default;
    DeckTreeProxyModel(const DeckTreeProxyModel&) = delete;
    DeckTreeProxyModel(DeckTreeProxyModel&&) = delete;
    DeckTreeProxyModel& operator=(const DeckTreeProxyModel&) = delete;
    DeckTreeProxyModel& operator=(DeckTreeProxyModel&&) = delete;

    void sort(int, Qt::SortOrder = Qt::AscendingOrder) noexcept override;
    [[nodiscard]] QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const noexcept override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const noexcept override;
};

}
