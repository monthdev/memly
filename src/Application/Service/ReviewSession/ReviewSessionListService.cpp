#include "Application/Service/ReviewSession/ReviewSessionListService.hpp"

#include <vector>

#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"

namespace Application::Service::ReviewSession {

[[nodiscard]] std::vector<ReviewSessionListService::ReviewSessionListRow> ReviewSessionListService::ReadReviewSessionList() {
    const std::vector<Infrastructure::Store::ReviewSession::ReviewSessionListStore::ReviewSessionListRow> StoreReviewSessionListRowVector{
        m_ReviewSessionListStore.ReadReviewSessionList()
    };
    std::vector<ReviewSessionListRow> ReviewSessionListRowVector{};
    ReviewSessionListRowVector.reserve(StoreReviewSessionListRowVector.size());
    for (const Infrastructure::Store::ReviewSession::ReviewSessionListStore::ReviewSessionListRow& StoreReviewSessionListRow :
         StoreReviewSessionListRowVector) {
        ReviewSessionListRowVector.emplace_back(StoreReviewSessionListRow.m_ReviewSessionId,
                                                StoreReviewSessionListRow.m_ReviewSessionName,
                                                StoreReviewSessionListRow.m_CreatedAtMillisecondsSinceEpoch,
                                                StoreReviewSessionListRow.m_LastCardReviewAtMillisecondsSinceEpoch);
    }
    return ReviewSessionListRowVector;
}

}
