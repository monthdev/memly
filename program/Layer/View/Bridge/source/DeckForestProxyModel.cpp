// Temporarily disabled during deck forest proxy model refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Bridge/DeckForestProxyModel.hpp"

#include <qstring.h>

#include "Memly/Exception/ExceptionBoundary.hpp"
#include "Memly/Model/DeckForestModel.hpp"

namespace Memly::Bridge {

DeckForestProxyModel::DeckForestProxyModel(Model::DeckForestModel& DeckForestModel, QObject* Parent)
    : QIdentityProxyModel{ Parent } {
    this->setSourceModel(&DeckForestModel);
}

void DeckForestProxyModel::sort(const int Column, const Qt::SortOrder SortOrder) noexcept {
    this->sourceModel()->sort(Column, SortOrder);
}

[[nodiscard]] QVariant DeckForestProxyModel::headerData(const int Section, const Qt::Orientation Orientation, const int Role) const noexcept {
    if (Orientation not_eq Qt::Horizontal or Role not_eq Qt::DisplayRole) {
        return QVariant{};
    }
    switch (Section) {
    case static_cast<int>(Model::DeckForestModel::ColumnEnum::DeckNameColumn):
        return QStringLiteral("Deck");
    case static_cast<int>(Model::DeckForestModel::ColumnEnum::SubtreeDueNowCountColumn):
        return QStringLiteral("Due Now");
    case static_cast<int>(Model::DeckForestModel::ColumnEnum::SubtreeByTodayCountColumn):
        return QStringLiteral("By Today");
    case static_cast<int>(Model::DeckForestModel::ColumnEnum::SubtreeTotalCountColumn):
        return QStringLiteral("Total");
    default:
        return QVariant{};
    }
}

[[nodiscard]] QHash<int, QByteArray> DeckForestProxyModel::roleNames() const noexcept {
    return Exception::TryCatchWrapper([&]() -> QHash<int, QByteArray> {
        static const QHash<int, QByteArray> s_DeckForestRoleNameByRoleQHash{
            {                                                                                         Qt::DisplayRole,QByteArrayLiteral("display")                                                                                                                      },
            {                              static_cast<int>(Model::DeckForestModel::RoleEnum::DeckIdRole),              QByteArrayLiteral("deckId") },
            {                        static_cast<int>(Model::DeckForestModel::RoleEnum::ParentDeckIdRole),        QByteArrayLiteral("parentDeckId") },
            {                            static_cast<int>(Model::DeckForestModel::RoleEnum::DeckNameRole),            QByteArrayLiteral("deckName") },
            {     static_cast<int>(Model::DeckForestModel::RoleEnum::CreatedAtMillisecondsSinceEpochRole),
             QByteArrayLiteral("createdAtMillisecondsSinceEpoch")                                                                                              },
            { static_cast<int>(Model::DeckForestModel::RoleEnum::LastUpdatedAtMillisecondsSinceEpochRole),
             QByteArrayLiteral("lastUpdatedAtMillisecondsSinceEpoch")                                                                                          },
            {                     static_cast<int>(Model::DeckForestModel::RoleEnum::SelfDueNowCountRole),     QByteArrayLiteral("selfDueNowCount") },
            {                    static_cast<int>(Model::DeckForestModel::RoleEnum::SelfByTodayCountRole),    QByteArrayLiteral("selfByTodayCount") },
            {                      static_cast<int>(Model::DeckForestModel::RoleEnum::SelfTotalCountRole),      QByteArrayLiteral("selfTotalCount") },
            {                  static_cast<int>(Model::DeckForestModel::RoleEnum::SubtreeDueNowCountRole),  QByteArrayLiteral("subtreeDueNowCount") },
            {                 static_cast<int>(Model::DeckForestModel::RoleEnum::SubtreeByTodayCountRole), QByteArrayLiteral("subtreeByTodayCount") },
            {                   static_cast<int>(Model::DeckForestModel::RoleEnum::SubtreeTotalCountRole),   QByteArrayLiteral("subtreeTotalCount") },
            {                  static_cast<int>(Model::DeckForestModel::RoleEnum::TargetLanguageCodeRole),  QByteArrayLiteral("targetLanguageCode") },
        };
        return s_DeckForestRoleNameByRoleQHash;
    });
}

}
#endif
