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

class DeckListStore final {
public:
    struct DeckListRowData {
        QString m_Id;
        QString m_Name;
        quint32 m_DueNow;
        quint32 m_ByToday;
        quint32 m_Total;
    };

    enum class DeckMutationStatus {
        Success,
        NameLengthError,
        DuplicateNameError,
        TargetLanguageCodeError
    };

    explicit DeckListStore(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    [[nodiscard]] QVector<DeckListRowData> ReadDeckList();
    [[nodiscard]] std::optional<std::int64_t> ReadDeckListNextRefreshDelayMilliseconds();
    [[nodiscard]] DeckMutationStatus CreateDeck(const QString&, quint8);
    [[nodiscard]] DeckMutationStatus UpdateDeckName(const QString&, const QString&);
    void DeleteDeck(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;

    [[nodiscard]] DeckMutationStatus
    HandleRecoverableDeckMutationError(const std::unique_ptr<duckdb::QueryResult>&) const;
};
}
