#include "Application/Service/ReviewSession/ReviewSessionListService.hpp"

#include <vector>

#include "Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"

namespace Application::Service::ReviewSession {

[[nodiscard]] std::vector<Application::Domain::ReviewSession::ReviewSessionListRow> ReviewSessionListService::ReadReviewSessionListRows() {
    return m_ReviewSessionListStore.ReadReviewSessionListRows();
}

}
