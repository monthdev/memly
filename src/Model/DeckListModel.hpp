#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

namespace Model {

class DeckListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    struct DeckListRow {
        QString m_Id;
        QString m_Name;
        quint32 m_DueNow;
        quint32 m_ByToday;
        quint32 m_Total;
    };

    enum Role { IdRole = Qt::UserRole + 1, NameRole, DueNowRole, ByTodayRole, TotalRole };

    explicit DeckListModel(QObject* Parent = nullptr) noexcept
        : QAbstractListModel{ Parent } {
    }

    int rowCount(const QModelIndex& Parent = QModelIndex{}) const override {
        if (Parent.isValid()) {
            return 0;
        }
        return static_cast<int>(m_DeckList.size());
    }

    QVariant data(const QModelIndex& Index, int Role) const override {
        if (!Index.isValid()) {
            return {};
        }

        const int Row{ Index.row() };
        if (Row < 0 || Row >= static_cast<int>(m_DeckList.size())) {
            return {};
        }

        const DeckListRow& DeckListRow{ m_DeckList[static_cast<qsizetype>(Row)] };

        switch (Role) {
        case IdRole:
            return DeckListRow.m_Id;
        case NameRole:
            return DeckListRow.m_Name;
        case DueNowRole:
            return DeckListRow.m_DueNow;
        case ByTodayRole:
            return DeckListRow.m_ByToday;
        case TotalRole:
            return DeckListRow.m_Total;
        default:
            return {};
        }
    }

    QHash<int, QByteArray> roleNames() const override {
        return {
            {      IdRole,      "id" },
            {    NameRole,    "name" },
            {  DueNowRole,  "dueNow" },
            { ByTodayRole, "byToday" },
            {   TotalRole,   "total" },
        };
    }

    void ReplaceAll(QVector<DeckListRow> DeckList) {
        beginResetModel();
        m_DeckList = std::move(DeckList);
        endResetModel();
    }

private:
    QVector<DeckListRow> m_DeckList;
};

}
