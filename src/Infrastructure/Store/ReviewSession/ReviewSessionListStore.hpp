#pragma once

#include <duckdb.hpp>

#include <QString>
#include <QVector>
#include <QtTypes>
#include <memory>
#include <optional>

#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionListStore final {
public:
    struct ReviewSessionListRow {
        QString m_ReviewSessionId;
        QString m_ReviewSessionName;
        qint64 m_CreatedAtMillisecondsSinceEpoch;
        std::optional<qint64> m_LastCardReviewAtMillisecondsSinceEpoch;
    };

    explicit ReviewSessionListStore(duckdb::Connection& DatabaseConnection)
        : m_ReadReviewSessionListPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::ReviewSession::Query::ReadReviewSessionListSql()) } {
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadReviewSessionListPreparedStatement);
    }

    ~ReviewSessionListStore() = default;

    [[nodiscard]] QVector<ReviewSessionListRow> ReadReviewSessionList();

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionListPreparedStatement;
};

}
