#include "Application/Service/ReviewSession/ReviewSessionListService.hpp"

#include <vector>

#include "Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionListStore.hpp"

namespace Application::Service::ReviewSession {

[[nodiscard]] auto ReviewSessionListService::ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow> {
    return m_ReviewSessionListStore.ReadReviewSessionListRows();
}

}
