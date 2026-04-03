#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <cstdint>
#include <memory>
#include <optional>

namespace duckdb {
class Connection;
class QueryResult;
}

namespace Service {

class DeckListService final : public QObject {
    Q_OBJECT

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

    explicit DeckListService(duckdb::Connection& DatabaseConnection,
                             QObject* Parent = nullptr) noexcept
        : QObject{ Parent }
        , m_DatabaseConnection{ DatabaseConnection } {
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
