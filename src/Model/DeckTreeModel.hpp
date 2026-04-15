#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

namespace Model {

class DeckTreeModel final : public QAbstractListModel {
    Q_OBJECT

public:
    struct DeckTreeRow {
        QString m_Id;
        QString m_ParentId;
        QString m_Name;
        quint32 m_DueNow;
        quint32 m_ByToday;
        quint32 m_Total;
    };

    enum Role {
        IdRole = Qt::UserRole + 1,
        ParentIdRole,
        NameRole,
        DueNowRole,
        ByTodayRole,
        TotalRole
    };

    explicit DeckTreeModel(QObject* Parent = nullptr) noexcept
        : QAbstractListModel{ Parent } {
    }

    int rowCount(const QModelIndex& Parent = QModelIndex{}) const override {
        if (Parent.isValid()) {
            return 0;
        }
        return static_cast<int>(m_DeckTree.size());
    }

    QVariant data(const QModelIndex& Index, int Role) const override {
        if (!Index.isValid()) {
            return {};
        }

        const int Row{ Index.row() };
        if (Row < 0 || Row >= static_cast<int>(m_DeckTree.size())) {
            return {};
        }

        const DeckTreeRow& DeckTreeRow{ m_DeckTree[static_cast<qsizetype>(Row)] };

        switch (Role) {
        case IdRole:
            return DeckTreeRow.m_Id;
        case ParentIdRole:
            return DeckTreeRow.m_ParentId;
        case NameRole:
            return DeckTreeRow.m_Name;
        case DueNowRole:
            return DeckTreeRow.m_DueNow;
        case ByTodayRole:
            return DeckTreeRow.m_ByToday;
        case TotalRole:
            return DeckTreeRow.m_Total;
        default:
            return {};
        }
    }

    QHash<int, QByteArray> roleNames() const override {
        return {
            {       IdRole,       "id" },
            { ParentIdRole, "parentId" },
            {     NameRole,     "name" },
            {   DueNowRole,   "dueNow" },
            {  ByTodayRole,  "byToday" },
            {    TotalRole,    "total" },
        };
    }

    void ReplaceAll(QVector<DeckTreeRow> DeckTree) {
        beginResetModel();
        m_DeckTree = std::move(DeckTree);
        endResetModel();
    }

private:
    QVector<DeckTreeRow> m_DeckTree;
};

}
