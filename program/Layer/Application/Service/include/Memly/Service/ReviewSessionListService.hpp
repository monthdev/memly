// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <vector>

#include "Memly/Domain/ReviewSessionListRow.hpp"

namespace Layer::Infrastructure::Repository {
class ReviewSessionRepository;
}

namespace Layer::Application::Service {

class ReviewSessionListService final {
private:
    Infrastructure::Repository::ReviewSessionRepository& m_ReviewSessionRepository;

public:
    explicit ReviewSessionListService(Infrastructure::Repository::ReviewSessionRepository&) noexcept;

    explicit ReviewSessionListService(const ReviewSessionListService&) = delete;
    auto operator=(const ReviewSessionListService&) -> ReviewSessionListService& = delete;

    explicit ReviewSessionListService(ReviewSessionListService&&) = delete;
    auto operator=(ReviewSessionListService&&) -> ReviewSessionListService& = delete;

    ~ReviewSessionListService() noexcept = default;

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Domain::ReviewSessionListRow>;
};

}
#endif
