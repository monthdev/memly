#include "Application/Service/ReviewSession/ReviewSessionListService.hpp"

#include <vector>

#include "Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"

namespace Application::Service::ReviewSession {

[[nodiscard]] std::vector<Domain::ReviewSession::ReviewSessionListRow> ReviewSessionListService::ReadReviewSessionList() {
    return m_ReviewSessionListStore.ReadReviewSessionList();
}

}
