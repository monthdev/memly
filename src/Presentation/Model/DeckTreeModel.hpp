#pragma once

#include <QAbstractItemModel>
#include <QByteArray>
#include <QHash>
#include <QString>
#include <QVariant>
#include <QVector>
#include <optional>

namespace Presentation::Model {

class DeckTreeModel final : public QAbstractItemModel {
    Q_OBJECT

public:
    struct DeckNodeData {
        QString m_DeckId;
        std::optional<QString> m_ParentDeckId;
        QString m_DeckName;
        qint64 m_CreatedAtMillisecondsSinceEpoch;
        std::optional<qint64> m_UpdatedAtMillisecondsSinceEpoch;
        quint32 m_SelfDueNowCount;
        quint32 m_SelfByTodayCount;
        quint32 m_SelfTotalCount;
        quint32 m_SubtreeDueNowCount;
        quint32 m_SubtreeByTodayCount;
        quint32 m_SubtreeTotalCount;
        quint8 m_TargetLanguageCode;
    };

    enum class RoleEnum : int {
        DeckIdRole = Qt::UserRole + 1,
        ParentDeckIdRole,
        DeckNameRole,
        CreatedAtMillisecondsSinceEpochRole,
        UpdatedAtMillisecondsSinceEpochRole,
        SelfDueNowCountRole,
        SelfByTodayCountRole,
        SelfTotalCountRole,
        SubtreeDueNowCountRole,
        SubtreeByTodayCountRole,
        SubtreeTotalCountRole,
        TargetLanguageCodeRole
    };

    enum class ColumnEnum : int {
        DeckNameColumn = 0,
        SubtreeDueNowCountColumn,
        SubtreeByTodayCountColumn,
        SubtreeTotalCountColumn
    };

    explicit DeckTreeModel(QObject* Parent = nullptr) noexcept
        : QAbstractItemModel{ Parent }
        , m_SortColumn{ -1 }
        , m_SortOrder{ Qt::AscendingOrder } {
    }

    [[nodiscard]] QModelIndex index(int Row, int Column, const QModelIndex& Parent = QModelIndex{}) const override {
        if (Row < 0 or Column < 0 or Parent.column() > 0 or Column >= columnCount(Parent)) {
            return QModelIndex{};
        }
        if (Parent.isValid() and TryGetDeckNode(Parent) == nullptr) {
            return QModelIndex{};
        }
        const QVector<qsizetype>& ChildDeckNodesQVector{ GetChildDeckNodeIndexes(Parent) };
        if (Row >= ChildDeckNodesQVector.size()) {
            return QModelIndex{};
        }
        const qsizetype ChildDeckNodeIndex{ ChildDeckNodesQVector.at(static_cast<qsizetype>(Row)) };
        return createIndex(Row, Column, static_cast<quintptr>(ChildDeckNodeIndex));
    }

    [[nodiscard]] QModelIndex parent(const QModelIndex& Index) const override {
        const DeckNode* CurrentDeckNode{ TryGetDeckNode(Index) };
        if (CurrentDeckNode == nullptr or CurrentDeckNode->m_ParentDeckNodeIndex == s_RootDeckNodeIndex) {
            return QModelIndex{};
        }
        const qsizetype ParentDeckNodeIndex{ CurrentDeckNode->m_ParentDeckNodeIndex };
        const DeckNode& ParentDeckNode{ m_DeckNodesQVector.at(ParentDeckNodeIndex) };
        return createIndex(static_cast<int>(ParentDeckNode.m_RowInParentIndex), 0, static_cast<quintptr>(ParentDeckNodeIndex));
    }

    [[nodiscard]] int rowCount(const QModelIndex& Parent = QModelIndex{}) const override {
        if (Parent.column() > 0) {
            return 0;
        }
        if (Parent.isValid() and TryGetDeckNode(Parent) == nullptr) {
            return 0;
        }
        return static_cast<int>(GetChildDeckNodeIndexes(Parent).size());
    }

    [[nodiscard]] int columnCount(const QModelIndex& Parent = QModelIndex{}) const override {
        if (Parent.isValid() and Parent.column() > 0) {
            return 0;
        }
        return static_cast<int>(ColumnEnum::SubtreeTotalCountColumn) + 1;
    }

    [[nodiscard]] QVariant data(const QModelIndex& Index, int Role) const override {
        const DeckNode* CurrentDeckNode{ TryGetDeckNode(Index) };
        if (CurrentDeckNode == nullptr) {
            return QVariant{};
        }
        switch (Role) {
        case Qt::DisplayRole:
            switch (Index.column()) {
            case static_cast<int>(ColumnEnum::DeckNameColumn):
                return CurrentDeckNode->m_DeckNodeData.m_DeckName;
            case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
                return CurrentDeckNode->m_DeckNodeData.m_SubtreeDueNowCount;
            case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
                return CurrentDeckNode->m_DeckNodeData.m_SubtreeByTodayCount;
            case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
                return CurrentDeckNode->m_DeckNodeData.m_SubtreeTotalCount;
            default:
                return QVariant{};
            }
        case static_cast<int>(RoleEnum::DeckIdRole):
            return CurrentDeckNode->m_DeckNodeData.m_DeckId;
        case static_cast<int>(RoleEnum::ParentDeckIdRole):
            if (not CurrentDeckNode->m_DeckNodeData.m_ParentDeckId.has_value()) {
                return QString{};
            }
            return CurrentDeckNode->m_DeckNodeData.m_ParentDeckId.value();
        case static_cast<int>(RoleEnum::DeckNameRole):
            return CurrentDeckNode->m_DeckNodeData.m_DeckName;
        case static_cast<int>(RoleEnum::CreatedAtMillisecondsSinceEpochRole):
            return CurrentDeckNode->m_DeckNodeData.m_CreatedAtMillisecondsSinceEpoch;
        case static_cast<int>(RoleEnum::UpdatedAtMillisecondsSinceEpochRole):
            if (not CurrentDeckNode->m_DeckNodeData.m_UpdatedAtMillisecondsSinceEpoch.has_value()) {
                return qint64{};
            }
            return CurrentDeckNode->m_DeckNodeData.m_UpdatedAtMillisecondsSinceEpoch.value();
        case static_cast<int>(RoleEnum::SelfDueNowCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_SelfDueNowCount;
        case static_cast<int>(RoleEnum::SelfByTodayCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_SelfByTodayCount;
        case static_cast<int>(RoleEnum::SelfTotalCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_SelfTotalCount;
        case static_cast<int>(RoleEnum::SubtreeDueNowCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_SubtreeDueNowCount;
        case static_cast<int>(RoleEnum::SubtreeByTodayCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_SubtreeByTodayCount;
        case static_cast<int>(RoleEnum::SubtreeTotalCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_SubtreeTotalCount;
        case static_cast<int>(RoleEnum::TargetLanguageCodeRole):
            return static_cast<quint32>(CurrentDeckNode->m_DeckNodeData.m_TargetLanguageCode);
        default:
            return QVariant{};
        }
    }

    [[nodiscard]] bool hasChildren(const QModelIndex& Parent = QModelIndex{}) const override {
        if (Parent.column() > 0) {
            return false;
        }
        return rowCount(Parent) > 0;
    }

    void sort(int Column, Qt::SortOrder Order = Qt::AscendingOrder) override {
        if (Column < static_cast<int>(ColumnEnum::DeckNameColumn) or Column > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
            return;
        }
        m_SortColumn = Column;
        m_SortOrder = Order;
        beginResetModel();
        ApplyCurrentSort();
        endResetModel();
    }

    [[nodiscard]] QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override {
        if (Orientation not_eq Qt::Horizontal or Role not_eq Qt::DisplayRole) {
            return {};
        }
        switch (Section) {
        case static_cast<int>(ColumnEnum::DeckNameColumn):
            return QStringLiteral("Deck");
        case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
            return QStringLiteral("Due Now");
        case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
            return QStringLiteral("By Today");
        case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
            return QStringLiteral("Total");
        default:
            return {};
        }
    }

    [[nodiscard]] Q_INVOKABLE QString GetColumnHeaderText(const int Column) const {
        return headerData(Column, Qt::Horizontal, Qt::DisplayRole).toString();
    }

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override {
        return {
            {                                                 Qt::DisplayRole,                         "display" },
            {                          static_cast<int>(RoleEnum::DeckIdRole),                          "deckId" },
            {                    static_cast<int>(RoleEnum::ParentDeckIdRole),                    "parentDeckId" },
            {                        static_cast<int>(RoleEnum::DeckNameRole),                        "deckName" },
            { static_cast<int>(RoleEnum::CreatedAtMillisecondsSinceEpochRole), "createdAtMillisecondsSinceEpoch" },
            { static_cast<int>(RoleEnum::UpdatedAtMillisecondsSinceEpochRole), "updatedAtMillisecondsSinceEpoch" },
            {                 static_cast<int>(RoleEnum::SelfDueNowCountRole),                 "selfDueNowCount" },
            {                static_cast<int>(RoleEnum::SelfByTodayCountRole),                "selfByTodayCount" },
            {                  static_cast<int>(RoleEnum::SelfTotalCountRole),                  "selfTotalCount" },
            {              static_cast<int>(RoleEnum::SubtreeDueNowCountRole),              "subtreeDueNowCount" },
            {             static_cast<int>(RoleEnum::SubtreeByTodayCountRole),             "subtreeByTodayCount" },
            {               static_cast<int>(RoleEnum::SubtreeTotalCountRole),               "subtreeTotalCount" },
            {              static_cast<int>(RoleEnum::TargetLanguageCodeRole),              "targetLanguageCode" },
        };
    }

    void ReplaceAll(QVector<DeckNodeData>);

private:
    struct DeckNode {
        DeckNodeData m_DeckNodeData;
        qsizetype m_ParentDeckNodeIndex;
        qsizetype m_RowInParentIndex;
        QVector<qsizetype> m_ChildDeckNodeIndexesQVector;
    };

    static constexpr qsizetype s_RootDeckNodeIndex{ -1 };

    QVector<DeckNode> m_DeckNodesQVector;
    QVector<qsizetype> m_RootDeckNodeIndexesQVector;
    QHash<QString, qsizetype> m_DeckNodeIndexByIdQHash;
    int m_SortColumn;
    Qt::SortOrder m_SortOrder;

    [[nodiscard]] const DeckNode* TryGetDeckNode(const QModelIndex&) const noexcept;
    [[nodiscard]] const DeckNode* TryGetDeckNode(const QString&) const noexcept;
    [[nodiscard]] const QVector<qsizetype>& GetChildDeckNodeIndexes(const QModelIndex&) const noexcept;
    void ApplyCurrentSort() noexcept;
    void SortSiblingDeckNodeIndexes(QVector<qsizetype>&) noexcept;
    void UpdateSiblingRowIndexes(qsizetype = s_RootDeckNodeIndex) noexcept;
    [[nodiscard]] int CompareDeckNodes(qsizetype, qsizetype) const noexcept;
};
}
