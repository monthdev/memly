#if 0
// Temporarily disabled during deck tree proxy model refactor.
#include "View/Bridge/ProxyModel/DeckTreeProxyModel.hpp"

#include <qstring.h>

#include "Support/Runtime/ExceptionBoundary.hpp"

namespace View::Bridge::ProxyModel {

void DeckTreeProxyModel::sort(const int Column, const Qt::SortOrder SortOrder) noexcept {
    sourceModel()->sort(Column, SortOrder);
}

[[nodiscard]] QVariant DeckTreeProxyModel::headerData(const int Section, const Qt::Orientation Orientation, const int Role) const noexcept {
    if (Orientation not_eq Qt::Horizontal or Role not_eq Qt::DisplayRole) {
        return {};
    }
    switch (Section) {
    case static_cast<int>(Presentation::Model::DeckTreeModel::ColumnEnum::DeckNameColumn):
        return QStringLiteral("Deck");
    case static_cast<int>(Presentation::Model::DeckTreeModel::ColumnEnum::SubtreeDueNowCountColumn):
        return QStringLiteral("Due Now");
    case static_cast<int>(Presentation::Model::DeckTreeModel::ColumnEnum::SubtreeByTodayCountColumn):
        return QStringLiteral("By Today");
    case static_cast<int>(Presentation::Model::DeckTreeModel::ColumnEnum::SubtreeTotalCountColumn):
        return QStringLiteral("Total");
    default:
        return {};
    }
}

[[nodiscard]] QHash<int, QByteArray> DeckTreeProxyModel::roleNames() const noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> QHash<int, QByteArray> {
        static const QHash<int, QByteArray> s_DeckTreeRoleNameByRoleQHash{
            {                                                                                         Qt::DisplayRole,QByteArrayLiteral("display")                                                                                                                      },
            {                              static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::DeckIdRole),              QByteArrayLiteral("deckId") },
            {                        static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::ParentDeckIdRole),        QByteArrayLiteral("parentDeckId") },
            {                            static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::DeckNameRole),            QByteArrayLiteral("deckName") },
            {     static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::CreatedAtMillisecondsSinceEpochRole),
             QByteArrayLiteral("createdAtMillisecondsSinceEpoch")                                                                                              },
            { static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::LastUpdatedAtMillisecondsSinceEpochRole),
             QByteArrayLiteral("lastUpdatedAtMillisecondsSinceEpoch")                                                                                          },
            {                     static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::SelfDueNowCountRole),     QByteArrayLiteral("selfDueNowCount") },
            {                    static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::SelfByTodayCountRole),    QByteArrayLiteral("selfByTodayCount") },
            {                      static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::SelfTotalCountRole),      QByteArrayLiteral("selfTotalCount") },
            {                  static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::SubtreeDueNowCountRole),  QByteArrayLiteral("subtreeDueNowCount") },
            {                 static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::SubtreeByTodayCountRole), QByteArrayLiteral("subtreeByTodayCount") },
            {                   static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::SubtreeTotalCountRole),   QByteArrayLiteral("subtreeTotalCount") },
            {                  static_cast<int>(Presentation::Model::DeckTreeModel::RoleEnum::TargetLanguageCodeRole),  QByteArrayLiteral("targetLanguageCode") },
        };
        return s_DeckTreeRoleNameByRoleQHash;
    });
}

}
#endif
