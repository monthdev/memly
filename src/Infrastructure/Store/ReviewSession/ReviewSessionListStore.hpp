#pragma once

#include <QString>
#include <QVector>
#include <QtTypes>
#include <memory>
#include <optional>

namespace duckdb {
class Connection;
class PreparedStatement;
}

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionListStore final {
public:
    struct ReviewSessionListRow {
        QString m_ReviewSessionId;
        QString m_ReviewSessionName;
        qint64 m_CreatedAtMillisecondsSinceEpoch;
        std::optional<qint64> m_LastCardReviewAtMillisecondsSinceEpoch;
    };

    explicit ReviewSessionListStore(duckdb::Connection&);
    ~ReviewSessionListStore();

    [[nodiscard]] QVector<ReviewSessionListRow> ReadReviewSessionList();

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionListPreparedStatement;
};

}
