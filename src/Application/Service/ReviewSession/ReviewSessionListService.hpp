#pragma once

#include <vector>

#include "Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionListStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionListService final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::Store::ReviewSession::ReviewSessionListStore& m_ReviewSessionListStore;

public:
    explicit ReviewSessionListService(Infrastructure::Store::ReviewSession::ReviewSessionListStore& ReviewSessionListStore) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ReviewSessionListStore{ ReviewSessionListStore } {
    }

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow>;
};

}
