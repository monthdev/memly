#pragma once

#include <vector>

#include "Application/Domain/ReviewSession/ReviewSessionListRow.hpp"

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionListStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionListService final {
private:
    Infrastructure::Store::ReviewSession::ReviewSessionListStore& m_ReviewSessionListStore;

public:
    explicit ReviewSessionListService(Infrastructure::Store::ReviewSession::ReviewSessionListStore& ReviewSessionListStore) noexcept
        : m_ReviewSessionListStore{ ReviewSessionListStore } {
    }

    explicit ReviewSessionListService(const ReviewSessionListService&) = delete;
    explicit ReviewSessionListService(ReviewSessionListService&&) = delete;
    auto operator=(const ReviewSessionListService&) -> ReviewSessionListService& = delete;
    auto operator=(ReviewSessionListService&&) -> ReviewSessionListService& = delete;

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow>;
};

}
