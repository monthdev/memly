// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <vector>

#include "Layer/Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Repository::ReviewSession {
class ReviewSessionRepository;
}

namespace Layer::Application::Service::ReviewSession {

class ReviewSessionListService final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::DuckDb::Repository::ReviewSession::ReviewSessionRepository& m_ReviewSessionRepository;

public:
    explicit ReviewSessionListService(Infrastructure::DuckDb::Repository::ReviewSession::ReviewSessionRepository& ReviewSessionRepository) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ReviewSessionRepository{ ReviewSessionRepository } {
    }

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Domain::ReviewSession::ReviewSessionListRow>;
};

}
#endif
