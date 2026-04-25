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
        QString m_DeckId;
        QString m_ParentDeckId;
        QString m_Name;
        quint32 m_DueNowCount;
        quint32 m_ByTodayCount;
        quint32 m_TotalCount;
        quint8 m_TargetLanguageCode;
    };

    enum class RoleEnum : int {
        IdRole = Qt::UserRole + 1,
        ParentIdRole,
        NameRole,
        DueNowCountRole,
        ByTodayCountRole,
        TotalCountRole,
        TargetLanguageCodeRole
    };

    enum class ColumnEnum : int {
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
        return static_cast<int>(ColumnEnum::TotalCountColumn) + 1;
    }

    [[nodiscard]] QVariant data(const QModelIndex& Index, int Role) const override {
        const DeckNode* CurrentDeckNode{ TryGetDeckNode(Index) };
        if (CurrentDeckNode == nullptr) {
            return QVariant{};
        }
        switch (Role) {
        case Qt::DisplayRole:
            switch (Index.column()) {
            case static_cast<int>(ColumnEnum::NameColumn):
                return CurrentDeckNode->m_DeckNodeData.m_Name;
            case static_cast<int>(ColumnEnum::DueNowCountColumn):
                return CurrentDeckNode->m_DeckNodeData.m_DueNowCount;
            case static_cast<int>(ColumnEnum::ByTodayCountColumn):
                return CurrentDeckNode->m_DeckNodeData.m_ByTodayCount;
            case static_cast<int>(ColumnEnum::TotalCountColumn):
                return CurrentDeckNode->m_DeckNodeData.m_TotalCount;
            default:
                return QVariant{};
            }
        case static_cast<int>(RoleEnum::IdRole):
            return CurrentDeckNode->m_DeckNodeData.m_DeckId;
        case static_cast<int>(RoleEnum::ParentIdRole):
            return CurrentDeckNode->m_DeckNodeData.m_ParentDeckId;
        case static_cast<int>(RoleEnum::NameRole):
            return CurrentDeckNode->m_DeckNodeData.m_Name;
        case static_cast<int>(RoleEnum::DueNowCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_DueNowCount;
        case static_cast<int>(RoleEnum::ByTodayCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_ByTodayCount;
        case static_cast<int>(RoleEnum::TotalCountRole):
            return CurrentDeckNode->m_DeckNodeData.m_TotalCount;
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
        if (Column < static_cast<int>(ColumnEnum::NameColumn) or Column > static_cast<int>(ColumnEnum::TotalCountColumn)) {
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
        case static_cast<int>(ColumnEnum::NameColumn):
            return QStringLiteral("Deck");
        case static_cast<int>(ColumnEnum::DueNowCountColumn):
            return QStringLiteral("Due Now");
        case static_cast<int>(ColumnEnum::ByTodayCountColumn):
            return QStringLiteral("By Today");
        case static_cast<int>(ColumnEnum::TotalCountColumn):
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
            {                                    Qt::DisplayRole,            "display" },
            {                 static_cast<int>(RoleEnum::IdRole),                 "id" },
            {           static_cast<int>(RoleEnum::ParentIdRole),           "parentId" },
            {               static_cast<int>(RoleEnum::NameRole),               "name" },
            {        static_cast<int>(RoleEnum::DueNowCountRole),        "dueNowCount" },
            {       static_cast<int>(RoleEnum::ByTodayCountRole),       "byTodayCount" },
            {         static_cast<int>(RoleEnum::TotalCountRole),         "totalCount" },
            { static_cast<int>(RoleEnum::TargetLanguageCodeRole), "targetLanguageCode" },
        };
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const DeckNodeData>> TryGetDeckNodeData(const QString&) const noexcept;
    [[nodiscard]] bool HasDuplicateSiblingName(const QString&, const QString&, const QString& = QString{}) const noexcept;
    [[nodiscard]] bool WouldReparentCreateCycle(const QString&, const QString&) const noexcept;
    [[nodiscard]] bool WouldReparentCreateTargetLanguageMismatch(const QString&, const QString&) const noexcept;

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
    void ValidateNoDeckNodeCycles(const QVector<DeckNode>&) const;
    void ValidateParentDeckNodeTargetLanguageCodes(const QVector<DeckNode>&) const;
    void ValidateUniqueSiblingDeckNodeNames(const QVector<DeckNode>&, const QVector<qsizetype>&) const;
};
}
