#pragma once

#include <QAbstractItemModel>
#include <QByteArray>
#include <QHash>
#include <QString>
#include <QVariant>
#include <QVector>
#include <optional>

namespace Presentation::Model {

class DeckTreeTableModel final : public QAbstractItemModel {
    Q_OBJECT

public:
    struct DeckNodeData {
        QString m_Id;
        QString m_ParentId;
        QString m_Name;
        quint32 m_DueNowCount;
        quint32 m_ByTodayCount;
        quint32 m_TotalCount;
        quint8 m_TargetLanguageCode;
    };

    enum Role {
        IdRole = Qt::UserRole + 1,
        ParentIdRole,
        NameRole,
        DueNowCountRole,
        ByTodayCountRole,
        TotalCountRole,
        TargetLanguageCodeRole
    };

    enum Column {
        NameColumn = 0,
        DueNowCountColumn,
        ByTodayCountColumn,
        TotalCountColumn
    };

    explicit DeckTreeTableModel(QObject* Parent = nullptr) noexcept
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
        return TotalCountColumn + 1;
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
            case DueNowCountColumn:
                return CurrentDeckNode->m_Data.m_DueNowCount;
            case ByTodayCountColumn:
                return CurrentDeckNode->m_Data.m_ByTodayCount;
            case TotalCountColumn:
                return CurrentDeckNode->m_Data.m_TotalCount;
            default:
                return QVariant{};
            }
        case IdRole:
            return CurrentDeckNode->m_Data.m_Id;
        case ParentIdRole:
            return CurrentDeckNode->m_Data.m_ParentId;
        case NameRole:
            return CurrentDeckNode->m_Data.m_Name;
        case DueNowCountRole:
            return CurrentDeckNode->m_Data.m_DueNowCount;
        case ByTodayCountRole:
            return CurrentDeckNode->m_Data.m_ByTodayCount;
        case TotalCountRole:
            return CurrentDeckNode->m_Data.m_TotalCount;
        case TargetLanguageCodeRole:
            return static_cast<quint32>(CurrentDeckNode->m_Data.m_TargetLanguageCode);
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
        if (Column < NameColumn or Column > TotalCountColumn) {
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
        case NameColumn:
            return QStringLiteral("Deck");
        case DueNowCountColumn:
            return QStringLiteral("Due Now");
        case ByTodayCountColumn:
            return QStringLiteral("By Today");
        case TotalCountColumn:
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
            {        Qt::DisplayRole,            "display" },
            {                 IdRole,                 "id" },
            {           ParentIdRole,           "parentId" },
            {               NameRole,               "name" },
            {        DueNowCountRole,        "dueNowCount" },
            {       ByTodayCountRole,       "byTodayCount" },
            {         TotalCountRole,         "totalCount" },
            { TargetLanguageCodeRole, "targetLanguageCode" },
        };
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const DeckNodeData>> TryGetDeckNodeData(const QString&) const noexcept;
    [[nodiscard]] bool HasDuplicateSiblingName(const QString&, const QString&, const QString& = QString{}) const noexcept;
    [[nodiscard]] bool WouldReparentCreateCycle(const QString&, const QString&) const noexcept;
    [[nodiscard]] bool WouldReparentCreateTargetLanguageMismatch(const QString&, const QString&) const noexcept;

    void ReplaceAll(QVector<DeckNodeData>);

private:
    struct DeckNode {
        DeckNodeData m_Data;
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
    void ValidateNoDeckNodeCycles(const QVector<DeckNode>&) const;
    void ValidateParentDeckNodeTargetLanguageCodes(const QVector<DeckNode>&) const;
    void ValidateUniqueSiblingDeckNodeNames(const QVector<DeckNode>&, const QVector<qsizetype>&) const;
};
}
