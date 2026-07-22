// Temporarily disabled during deck forest proxy model refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qabstractitemmodel.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qidentityproxymodel.h>
#include <qqmlintegration.h>
#include <qvariant.h>

#include "Presentation/Model/DeckForestModel.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace View::Bridge::ProxyModel {

class DeckForestProxyModel final : public QIdentityProxyModel, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit DeckForestProxyModel(Presentation::Model::DeckForestModel& DeckForestModel, QObject* Parent = nullptr)
        : QIdentityProxyModel{ Parent }
        , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{} {
        setSourceModel(&DeckForestModel);
    }

    void sort(int, Qt::SortOrder = Qt::AscendingOrder) noexcept override;
    [[nodiscard]] QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const noexcept override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const noexcept override;
};

}
#endif
