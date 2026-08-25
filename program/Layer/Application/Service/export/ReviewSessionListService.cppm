export module Memly.Service.ReviewSessionListService;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <vector>

import Memly.Domain.ReviewSessionListRow;
import Memly.Repository.ReviewSessionRepository;

export namespace Memly::Service {

class ReviewSessionListService final {
private:
    Repository::ReviewSessionRepository& m_ReviewSessionRepository;

public:
    explicit ReviewSessionListService(Repository::ReviewSessionRepository&) noexcept;

    explicit ReviewSessionListService(const ReviewSessionListService&) = delete;
    auto operator=(const ReviewSessionListService&) -> ReviewSessionListService& = delete;

    explicit ReviewSessionListService(ReviewSessionListService&&) = delete;
    auto operator=(ReviewSessionListService&&) -> ReviewSessionListService& = delete;

    ~ReviewSessionListService() noexcept = default;

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Domain::ReviewSessionListRow>;
};

}
#endif
