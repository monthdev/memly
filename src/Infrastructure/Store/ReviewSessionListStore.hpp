#pragma once

#include <QString>
#include <QVector>
#include <QtTypes>
#include <memory>

namespace duckdb {
class Connection;
class PreparedStatement;
}

namespace Infrastructure::Store {

class ReviewSessionListStore final {
public:
    struct ReviewSessionListRow {
        QString m_ReviewSessionId;
        QString m_ReviewSessionName;
        qint64 m_LastOpenedAtMillisecondsSinceEpoch;
    };

    explicit ReviewSessionListStore(duckdb::Connection&);
    ~ReviewSessionListStore();

    [[nodiscard]] QVector<ReviewSessionListRow> ReadReviewSessionList();

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionListPreparedStatement;
};

}
