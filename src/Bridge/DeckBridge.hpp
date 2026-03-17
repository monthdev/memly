#pragma once

#include <duckdb.hpp>

#include <QObject>
#include <QString>

namespace Bridge {
class DeckBridge final : public QObject {
    Q_OBJECT

public:
    explicit DeckBridge(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    Q_INVOKABLE QString NameLengthErrorMessage();
    Q_INVOKABLE QString DuplicateErrorMessage();

    Q_INVOKABLE QString AddDeck(const QString& Name);
    // Q_INVOKABLE QString EditDeck(const QVariantMap& Deck);
    // Q_INVOKABLE void DeleteDeck();
    // Q_INVOKABLE QStringList GetDeckList();

private:
    duckdb::Connection& m_DatabaseConnection;
};
}
