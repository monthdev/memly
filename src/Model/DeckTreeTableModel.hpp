#pragma once

#include <QAbstractItemModel>
#include <QByteArray>
#include <QHash>
#include <QString>
#include <QVariant>
#include <QVector>

namespace Model {

class DeckTreeTableModel final : public QAbstractItemModel {
    Q_OBJECT

public:
    struct DeckNodeData {
        QString m_Id;
        QString m_ParentId;
        QString m_Name;
        quint32 m_DueNow;
        quint32 m_ByToday;
        quint32 m_Total;
        quint8 m_TargetLanguageCode;
    };

    enum Role {
        IdRole = Qt::UserRole + 1,
        ParentIdRole,
        NameRole,
        DueNowRole,
        ByTodayRole,
        TotalRole,
        TargetLanguageCodeRole
    };

    enum Column {
        NameColumn = 0,
        DueNowColumn,
        ByTodayColumn,
        TotalColumn
    };

    explicit DeckTreeTableModel(QObject* Parent = nullptr) noexcept
        : QAbstractItemModel{ Parent } {
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
        const DeckNode& ParentDeckNode{ m_DeckNodes.at(ParentDeckNodeIndex) };
        return createIndex(static_cast<int>(ParentDeckNode.m_RowInParent), 0, static_cast<quintptr>(ParentDeckNodeIndex));
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
        return TotalColumn + 1;
    }

    [[nodiscard]] QVariant data(const QModelIndex& Index, int Role) const override {
        const DeckNode* CurrentDeckNode{ TryGetDeckNode(Index) };
        if (CurrentDeckNode == nullptr) {
            return QVariant{};
        }
        switch (Role) {
        case Qt::DisplayRole:
            switch (Index.column()) {
            case NameColumn:
                return CurrentDeckNode->m_Data.m_Name;
            case DueNowColumn:
                return CurrentDeckNode->m_Data.m_DueNow;
            case ByTodayColumn:
                return CurrentDeckNode->m_Data.m_ByToday;
            case TotalColumn:
                return CurrentDeckNode->m_Data.m_Total;
            default:
                return QVariant{};
            }
        case Qt::TextAlignmentRole:
            switch (Index.column()) {
            case NameColumn:
                return static_cast<int>(Qt::AlignLeft bitor Qt::AlignVCenter);
            case DueNowColumn:
            case ByTodayColumn:
            case TotalColumn:
                return static_cast<int>(Qt::AlignRight bitor Qt::AlignVCenter);
            default:
                return QVariant{};
            }
        case IdRole:
            return CurrentDeckNode->m_Data.m_Id;
        case ParentIdRole:
            return CurrentDeckNode->m_Data.m_ParentId;
        case NameRole:
            return CurrentDeckNode->m_Data.m_Name;
        case DueNowRole:
            return CurrentDeckNode->m_Data.m_DueNow;
        case ByTodayRole:
            return CurrentDeckNode->m_Data.m_ByToday;
        case TotalRole:
            return CurrentDeckNode->m_Data.m_Total;
        case TargetLanguageCodeRole:
            return static_cast<quint32>(CurrentDeckNode->m_Data.m_TargetLanguageCode);
        case Qt::DecorationRole:
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::FontRole:
        case Qt::ForegroundRole:
        case Qt::BackgroundRole:
        case Qt::InitialSortOrderRole:
        case Qt::SizeHintRole:
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
        if (Column < NameColumn or Column > TotalColumn) {
            return;
        }

        m_SortColumn = Column;
        m_SortOrder = Order;

        beginResetModel();
        ApplyCurrentSort();
        endResetModel();
    }

    [[nodiscard]] QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override {
        if (Orientation not_eq Qt::Horizontal) {
            return {};
        }

        switch (Role) {
        case Qt::DisplayRole:
            switch (Section) {
            case NameColumn:
                return QStringLiteral("Deck");
            case DueNowColumn:
                return QStringLiteral("Due Now");
            case ByTodayColumn:
                return QStringLiteral("By Today");
            case TotalColumn:
                return QStringLiteral("Total");
            default:
                return {};
            }
        case Qt::TextAlignmentRole:
            switch (Section) {
            case NameColumn:
                return static_cast<int>(Qt::AlignLeft bitor Qt::AlignVCenter);
            case DueNowColumn:
            case ByTodayColumn:
            case TotalColumn:
                return static_cast<int>(Qt::AlignRight bitor Qt::AlignVCenter);
            default:
                return {};
            }
        default:
            return {};
        }
    }

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override {
        return {
            {        Qt::DisplayRole,            "display" },
            {                 IdRole,                 "id" },
            {           ParentIdRole,           "parentId" },
            {               NameRole,               "name" },
            {             DueNowRole,             "dueNow" },
            {            ByTodayRole,            "byToday" },
            {              TotalRole,              "total" },
            { TargetLanguageCodeRole, "targetLanguageCode" },
        };
    }

    [[nodiscard]] bool WouldCreateDeckHaveDuplicateSiblingName(const QString&, const QString&) const noexcept;
    [[nodiscard]] bool WouldMoveDeckCreateCycle(const QString&, const QString&) const noexcept;
    [[nodiscard]] bool WouldMoveDeckHaveDuplicateSiblingName(const QString&, const QString&) const noexcept;
    [[nodiscard]] bool WouldUpdateDeckNameHaveDuplicateSiblingName(const QString&, const QString&) const noexcept;

    void ReplaceAll(QVector<DeckNodeData>);

private:
    struct DeckNode {
        DeckNodeData m_Data;
        qsizetype m_ParentDeckNodeIndex{ -1 };
        qsizetype m_RowInParent{ -1 };
        QVector<qsizetype> m_ChildDeckNodeIndexes;
    };

    static constexpr qsizetype s_RootDeckNodeIndex{ -1 };

    QVector<DeckNode> m_DeckNodes;
    QVector<qsizetype> m_RootDeckNodeIndexes;
    QHash<QString, qsizetype> m_DeckNodeIndexById;
    int m_SortColumn{ -1 };
    Qt::SortOrder m_SortOrder{ Qt::AscendingOrder };

    [[nodiscard]] const DeckNode* TryGetDeckNode(const QModelIndex&) const noexcept;
    [[nodiscard]] const DeckNode* TryGetDeckNode(const QString&) const noexcept;
    [[nodiscard]] const QVector<qsizetype>& GetChildDeckNodeIndexes(const QModelIndex&) const noexcept;
    [[nodiscard]] bool HasDuplicateSiblingName(const QString&, const QString&, const QString& = {}) const noexcept;
    void ApplyCurrentSort() noexcept;
    void SortSiblingDeckNodeIndexes(QVector<qsizetype>&) noexcept;
    void UpdateSiblingRowIndexes(qsizetype = s_RootDeckNodeIndex) noexcept;
    [[nodiscard]] int CompareDeckNodes(qsizetype, qsizetype) const noexcept;
};
}
