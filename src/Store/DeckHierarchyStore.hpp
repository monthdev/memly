#pragma once

#include <QString>
#include <QVector>
#include <cstdint>
#include <memory>
#include <optional>

namespace duckdb {
class Connection;
class QueryResult;
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

    enum class DeckMutationStatus {
        Success,
        NameLengthError,
        DuplicateNameError,
        TargetLanguageCodeError,
        ParentDeckError,
        CycleDetectionError
    };

    explicit DeckHierarchyStore(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    [[nodiscard]] DeckHierarchyViewSnapshot ReadDeckHierarchyViewSnapshot();
    [[nodiscard]] DeckMutationStatus CreateDeck(const QString&, quint8, const QString& = QString{});
    [[nodiscard]] DeckMutationStatus MoveDeck(const QString&, const QString& = QString{});
    [[nodiscard]] DeckMutationStatus UpdateDeckName(const QString&, const QString&);
    void DeleteDeck(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;

    [[nodiscard]] DeckMutationStatus HandleRecoverableDeckMutationError(const std::unique_ptr<duckdb::QueryResult>&) const;
};
}
