// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <vector>

#include "Layer/Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Store::ReviewSession {
class ReviewSessionListStore;
}

namespace Layer::Application::Service::ReviewSession {

class ReviewSessionListService final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::Store::ReviewSession::ReviewSessionListStore& m_ReviewSessionListStore;

public:
    explicit ReviewSessionListService(Infrastructure::Store::ReviewSession::ReviewSessionListStore& ReviewSessionListStore) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ReviewSessionListStore{ ReviewSessionListStore } {
    }

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Domain::ReviewSession::ReviewSessionListRow>;
};

}
#endif
