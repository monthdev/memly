#pragma once

#include <QString>
#include <QVector>
#include <QtTypes>

namespace duckdb {
class Connection;
}

namespace Infrastructure::Store {

class DeckTreeStore final {
public:
    struct DeckTreeRow {
        QString m_DeckId;
        QString m_ParentDeckId;
        QString m_DeckName;
        quint32 m_DueNowCount;
        quint32 m_ByTodayCount;
        quint32 m_TotalCount;
        quint8 m_TargetLanguageCode;
    };

    explicit DeckTreeStore(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    [[nodiscard]] QVector<DeckTreeRow> ReadDeckTreeSnapshot(const qint64);

private:
    duckdb::Connection& m_DatabaseConnection;
};
}
