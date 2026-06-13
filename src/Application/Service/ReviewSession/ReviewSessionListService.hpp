#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionListStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionListService final {
public:
    struct ReviewSessionListRow {
        std::string m_ReviewSessionId;
        std::string m_ReviewSessionName;
        std::int64_t m_CreatedAtMillisecondsSinceEpoch;
        std::optional<std::int64_t> m_LastCardReviewAtMillisecondsSinceEpoch;
    };

    explicit ReviewSessionListService(Infrastructure::Store::ReviewSession::ReviewSessionListStore& ReviewSessionListStore) noexcept
        : m_ReviewSessionListStore{ ReviewSessionListStore } {
    }

    ~ReviewSessionListService() noexcept = default;
    ReviewSessionListService(const ReviewSessionListService&) = delete;
    ReviewSessionListService(ReviewSessionListService&&) = delete;
    ReviewSessionListService& operator=(const ReviewSessionListService&) = delete;
    ReviewSessionListService& operator=(ReviewSessionListService&&) = delete;

    [[nodiscard]] std::vector<ReviewSessionListRow> ReadReviewSessionList();

private:
    Infrastructure::Store::ReviewSession::ReviewSessionListStore& m_ReviewSessionListStore;
};

}
