#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "Model/DeckListModel.hpp"

namespace duckdb {
class Connection;
class QueryResult;
}

namespace Bridge {

class DeckListBridge final : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckListModel* deckList READ GetDeckList CONSTANT)
public:
    explicit DeckListBridge(duckdb::Connection& DatabaseConnection,
                            QObject* Parent = nullptr) noexcept
        : QObject{ Parent }
        , m_DatabaseConnection{ DatabaseConnection }
        , m_DeckListRefreshTimer{}
        , m_DeckList{ this } {
        m_DeckListRefreshTimer.setSingleShot(true);
        connect(&m_DeckListRefreshTimer, &QTimer::timeout, this, &DeckListBridge::RefreshDeckList);
    }

    Model::DeckListModel* GetDeckList() noexcept {
        return &m_DeckList;
    }

    [[nodiscard]] Q_INVOKABLE QString NameLengthErrorMessage();
    [[nodiscard]] Q_INVOKABLE QString DuplicateNameErrorMessage();
    [[nodiscard]] Q_INVOKABLE QString TargetLanguageCodeErrorMessage();
    [[nodiscard]] [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString&, const quint8);
    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString&, const QString&);
    Q_INVOKABLE void onDecksPageActivated();
    Q_INVOKABLE void onDecksPageDeactivated();
    Q_INVOKABLE void DeleteDeck(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;
    QTimer m_DeckListRefreshTimer;
    Model::DeckListModel m_DeckList;

    [[nodiscard]] QString
    HandleQueryResultRecoverableError(const std::unique_ptr<duckdb::QueryResult>&);
    void ScheduleNextDeckListRefresh();
    void RefreshDeckList();
};
}
