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

    ~ReviewSessionListService() noexcept = default;
    ReviewSessionListService(const ReviewSessionListService&) = delete;
    ReviewSessionListService(ReviewSessionListService&&) = delete;
    ReviewSessionListService& operator=(const ReviewSessionListService&) = delete;
    ReviewSessionListService& operator=(ReviewSessionListService&&) = delete;

    [[nodiscard]] QVector<ReviewSessionListRow> ReadReviewSessionList();

private:
    Infrastructure::Store::ReviewSession::ReviewSessionListStore& m_ReviewSessionListStore;
};

}
