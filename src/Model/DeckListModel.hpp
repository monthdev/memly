#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

namespace Model {

class DeckListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    struct DeckItem {
        QString m_Id;
        QString m_Name;
    };

    enum Role {
        IdRole = Qt::UserRole + 1,
        NameRole,
    };

    explicit DeckListModel(QObject* Parent = nullptr) noexcept
        : QAbstractListModel{ Parent } {
    }

    int rowCount(const QModelIndex& Parent = QModelIndex{}) const override {
        if (Parent.isValid()) {
            return 0;
        }
        return static_cast<int>(m_Decks.size());
    }

    QVariant data(const QModelIndex& Index, int Role) const override {
        if (!Index.isValid()) {
            return {};
        }

        const int Row{ Index.row() };
        if (Row < 0 || Row >= static_cast<int>(m_Decks.size())) {
            return {};
        }

        const DeckItem& Deck{ m_Decks[static_cast<qsizetype>(Row)] };

        switch (Role) {
        case IdRole:
            return Deck.m_Id;
        case NameRole:
            return Deck.m_Name;
        default:
            return {};
        }
    }

    QHash<int, QByteArray> roleNames() const override {
        return {
            {   IdRole,   "id" },
            { NameRole, "name" },
        };
    }

    void ReplaceAll(QVector<DeckItem> Decks) {
        beginResetModel();
        m_Decks = std::move(Decks);
        endResetModel();
    }

    void Clear() {
        beginResetModel();
        m_Decks.clear();
        endResetModel();
    }

private:
    QVector<DeckItem> m_Decks;
};

}
