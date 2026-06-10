#include "Application/Service/ReviewSession/ReviewSessionListService.hpp"

#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"

namespace Application::Service::ReviewSession {

[[nodiscard]] QVector<ReviewSessionListService::ReviewSessionListRow> ReviewSessionListService::ReadReviewSessionList() {
    const QVector<Infrastructure::Store::ReviewSession::ReviewSessionListStore::ReviewSessionListRow> StoreReviewSessionListRowQVector{
        m_ReviewSessionListStore.ReadReviewSessionList()
    };
    QVector<ReviewSessionListRow> ReviewSessionListRowQVector{};
    ReviewSessionListRowQVector.reserve(StoreReviewSessionListRowQVector.size());
    for (const Infrastructure::Store::ReviewSession::ReviewSessionListStore::ReviewSessionListRow& StoreReviewSessionListRow :
         StoreReviewSessionListRowQVector) {
        ReviewSessionListRowQVector.emplace_back(StoreReviewSessionListRow.m_ReviewSessionId,
                                                 StoreReviewSessionListRow.m_ReviewSessionName,
                                                 StoreReviewSessionListRow.m_CreatedAtMillisecondsSinceEpoch,
                                                 StoreReviewSessionListRow.m_LastCardReviewAtMillisecondsSinceEpoch);
    }
    return ReviewSessionListRowQVector;
}

}
