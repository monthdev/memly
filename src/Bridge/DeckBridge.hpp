#pragma once

#include <duckdb.hpp>

#include <QObject>
#include <QString>

namespace Bridge {
class DeckBridge final : public QObject {
    Q_OBJECT

public:
    explicit DeckBridge(duckdb::Connection& Connection) noexcept
        : m_Connection{ Connection } {
    }

    Q_INVOKABLE QString AddDeck(const QString& Name);
    // Q_INVOKABLE QStringList GetDeckList();
    // Q_INVOKABLE void DeleteDeck();
    // Q_INVOKABLE QString EditDeck(const QVariantMap& Deck);

private:
    duckdb::Connection& m_Connection;
};
}
