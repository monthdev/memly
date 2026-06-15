#pragma once

#include <vector>

#include "Domain/ReviewSession/ReviewSessionListRow.hpp"

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionListStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionListService final {
public:
    explicit ReviewSessionListService(Infrastructure::Store::ReviewSession::ReviewSessionListStore& ReviewSessionListStore) noexcept
        : m_ReviewSessionListStore{ ReviewSessionListStore } {
    }

    ~ReviewSessionListService() noexcept = default;
    ReviewSessionListService(const ReviewSessionListService&) = delete;
    ReviewSessionListService(ReviewSessionListService&&) = delete;
    ReviewSessionListService& operator=(const ReviewSessionListService&) = delete;
    ReviewSessionListService& operator=(ReviewSessionListService&&) = delete;

    [[nodiscard]] std::vector<Domain::ReviewSession::ReviewSessionListRow> ReadReviewSessionList();

private:
    Infrastructure::Store::ReviewSession::ReviewSessionListStore& m_ReviewSessionListStore;
};

}
