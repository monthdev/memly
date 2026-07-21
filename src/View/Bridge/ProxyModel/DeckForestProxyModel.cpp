// Temporarily disabled during deck forest proxy model refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "View/Bridge/ProxyModel/DeckForestProxyModel.hpp"

#include <qstring.h>

#include "Support/Runtime/ExceptionBoundary.hpp"

namespace View::Bridge::ProxyModel {

void DeckForestProxyModel::sort(const int Column, const Qt::SortOrder SortOrder) noexcept {
    sourceModel()->sort(Column, SortOrder);
}

[[nodiscard]] QVariant DeckForestProxyModel::headerData(const int Section, const Qt::Orientation Orientation, const int Role) const noexcept {
    if (Orientation not_eq Qt::Horizontal or Role not_eq Qt::DisplayRole) {
        return {};
    }
    switch (Section) {
    case static_cast<int>(Presentation::Model::DeckForestModel::ColumnEnum::DeckNameColumn):
        return QStringLiteral("Deck");
    case static_cast<int>(Presentation::Model::DeckForestModel::ColumnEnum::SubtreeDueNowCountColumn):
        return QStringLiteral("Due Now");
    case static_cast<int>(Presentation::Model::DeckForestModel::ColumnEnum::SubtreeByTodayCountColumn):
        return QStringLiteral("By Today");
    case static_cast<int>(Presentation::Model::DeckForestModel::ColumnEnum::SubtreeTotalCountColumn):
        return QStringLiteral("Total");
    default:
        return {};
    }
}

[[nodiscard]] QHash<int, QByteArray> DeckForestProxyModel::roleNames() const noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> QHash<int, QByteArray> {
        static const QHash<int, QByteArray> s_DeckForestRoleNameByRoleQHash{
            {                                                                                         Qt::DisplayRole,QByteArrayLiteral("display")                                                                                                                      },
            {                              static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::DeckIdRole),              QByteArrayLiteral("deckId") },
            {                        static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::ParentDeckIdRole),        QByteArrayLiteral("parentDeckId") },
            {                            static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::DeckNameRole),            QByteArrayLiteral("deckName") },
            {     static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::CreatedAtMillisecondsSinceEpochRole),
             QByteArrayLiteral("createdAtMillisecondsSinceEpoch")                                                                                              },
            { static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::LastUpdatedAtMillisecondsSinceEpochRole),
             QByteArrayLiteral("lastUpdatedAtMillisecondsSinceEpoch")                                                                                          },
            {                     static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::SelfDueNowCountRole),     QByteArrayLiteral("selfDueNowCount") },
            {                    static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::SelfByTodayCountRole),    QByteArrayLiteral("selfByTodayCount") },
            {                      static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::SelfTotalCountRole),      QByteArrayLiteral("selfTotalCount") },
            {                  static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::SubtreeDueNowCountRole),  QByteArrayLiteral("subtreeDueNowCount") },
            {                 static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::SubtreeByTodayCountRole), QByteArrayLiteral("subtreeByTodayCount") },
            {                   static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::SubtreeTotalCountRole),   QByteArrayLiteral("subtreeTotalCount") },
            {                  static_cast<int>(Presentation::Model::DeckForestModel::RoleEnum::TargetLanguageCodeRole),  QByteArrayLiteral("targetLanguageCode") },
        };
        return s_DeckForestRoleNameByRoleQHash;
    });
}

}
#endif
