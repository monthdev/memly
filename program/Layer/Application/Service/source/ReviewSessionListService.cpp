// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Service/ReviewSessionListService.hpp"

#include <vector>

#include "Memly/Domain/ReviewSessionListRow.hpp"
#include "Memly/Repository/ReviewSessionRepository.hpp"

namespace Layer::Application::Service {

ReviewSessionListService::ReviewSessionListService(
    Infrastructure::Repository::ReviewSessionRepository& ReviewSessionRepository) noexcept
    : m_ReviewSessionRepository{ ReviewSessionRepository } {
}

[[nodiscard]] auto ReviewSessionListService::ReadReviewSessionListRows() -> std::vector<Domain::ReviewSessionListRow> {
    return this->m_ReviewSessionRepository.ReadReviewSessionListRows();
}

}
#endif
