// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Application/Service/ReviewSession/ReviewSessionListService.hpp"

#include <vector>

#include "Layer/Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Layer/Infrastructure/Persistence/Store/ReviewSession/ReviewSessionListStore.hpp"

namespace Layer::Application::Service::ReviewSession {

[[nodiscard]] auto ReviewSessionListService::ReadReviewSessionListRows() -> std::vector<Domain::ReviewSession::ReviewSessionListRow> {
    return this->m_ReviewSessionListStore.ReadReviewSessionListRows();
}

}
#endif
