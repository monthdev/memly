#pragma once

#include <duckdb.hpp>

#include <QObject>
#include <QString>

#include "Bridge/FatalErrorBridge.hpp"

namespace Bridge {
class DeckBridge final : public QObject {
    Q_OBJECT

public:
    explicit DeckBridge(duckdb::Connection& Connection,
                        Bridge::FatalErrorBridge& FatalErrorBridge) noexcept
        : m_Connection{ Connection }
        , m_FatalErrorBridge{ FatalErrorBridge } {
    }

    Q_INVOKABLE QString AddDeck(const QString& Name);
    // Q_INVOKABLE QStringList GetDeckList();
    // Q_INVOKABLE void DeleteDeck();
    // Q_INVOKABLE QString EditDeck(const QVariantMap& Deck);

private:
    duckdb::Connection& m_Connection;
    Bridge::FatalErrorBridge& m_FatalErrorBridge;
};
}
