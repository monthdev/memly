#pragma once

#include <QString>
#include <QVector>
#include <cstdint>
#include <optional>

namespace duckdb {
class Connection;
}

namespace Store {

class DeckHierarchyStore final {
public:
    struct DeckHierarchyRow {
        QString m_Id;
        QString m_ParentId;
        QString m_Name;
        quint32 m_DueNowCount;
        quint32 m_ByTodayCount;
        quint32 m_TotalCount;
        quint8 m_TargetLanguageCode;
    };

    struct DeckHierarchyViewSnapshot {
        QVector<DeckHierarchyRow> m_DeckHierarchyRowQVector;
        std::optional<std::int64_t> m_NextRefreshAtMillisecondsSinceEpoch;
    };

    explicit DeckHierarchyStore(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    [[nodiscard]] DeckHierarchyViewSnapshot ReadDeckHierarchyViewSnapshot();

private:
    duckdb::Connection& m_DatabaseConnection;
};
}
