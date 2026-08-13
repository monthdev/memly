// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <vector>

#include "Memly/Domain/ReviewSessionListRow.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Repository {
class ReviewSessionRepository;
}

namespace Layer::Application::Service {

class ReviewSessionListService final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::Repository::ReviewSessionRepository& m_ReviewSessionRepository;

public:
    explicit ReviewSessionListService(Infrastructure::Repository::ReviewSessionRepository&) noexcept;

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Domain::ReviewSessionListRow>;
};

}
#endif
