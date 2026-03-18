#pragma once

#include <duckdb.hpp>

#include <QObject>
#include <QString>

#include "Model/DeckListModel.hpp"

namespace Bridge {

class DeckBridge final : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckListModel* deckList READ GetDeckList CONSTANT)
public:
    explicit DeckBridge(duckdb::Connection& DatabaseConnection,
                        QObject* Parent = nullptr) noexcept
        : QObject{ Parent }
        , m_DatabaseConnection{ DatabaseConnection }
        , m_DeckList{ this } {
        ReadDeckTable();
    }

    Model::DeckListModel* GetDeckList() noexcept {
        return &m_DeckList;
    }

    Q_INVOKABLE QString NameLengthErrorMessage();
    Q_INVOKABLE QString DuplicateErrorMessage();

    Q_INVOKABLE QString CreateDeck(const QString&);
    Q_INVOKABLE QString UpdateDeck(const Model::DeckListModel::DeckItem&);
    Q_INVOKABLE void DeleteDeck(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;
    Model::DeckListModel m_DeckList;

    [[nodiscard]] QString
    HandleQueryResult(const std::unique_ptr<duckdb::QueryResult>&);
    void ReadDeckTable();
};
}
