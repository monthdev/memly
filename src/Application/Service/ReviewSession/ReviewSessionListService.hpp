#pragma once

#include <QString>
#include <QVector>
#include <QtTypes>
#include <optional>

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionListStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionListService final {
public:
    struct ReviewSessionListRow {
        QString m_ReviewSessionId;
        QString m_ReviewSessionName;
        qint64 m_CreatedAtMillisecondsSinceEpoch;
        std::optional<qint64> m_LastCardReviewAtMillisecondsSinceEpoch;
    };

    explicit ReviewSessionListService(Infrastructure::Store::ReviewSession::ReviewSessionListStore& ReviewSessionListStore) noexcept
        : m_ReviewSessionListStore{ ReviewSessionListStore } {
    }

    ~ReviewSessionListService() = default;

    [[nodiscard]] QVector<ReviewSessionListRow> ReadReviewSessionList();

private:
    Infrastructure::Store::ReviewSession::ReviewSessionListStore& m_ReviewSessionListStore;
};

}
