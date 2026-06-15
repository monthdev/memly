#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace Domain::ReviewSession {

struct ReviewSessionListRow {
    ReviewSessionListRow(std::string ReviewSessionId,
                         std::string ReviewSessionName,
                         const std::int64_t CreatedAtMillisecondsSinceEpoch,
                         std::optional<std::int64_t> UpdatedAtMillisecondsSinceEpoch,
                         std::optional<std::int64_t> LastCardReviewAtMillisecondsSinceEpoch)
        : m_ReviewSessionId{ std::move(ReviewSessionId) }
        , m_ReviewSessionName{ std::move(ReviewSessionName) }
        , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
        , m_UpdatedAtMillisecondsSinceEpoch{ std::move(UpdatedAtMillisecondsSinceEpoch) }
        , m_LastCardReviewAtMillisecondsSinceEpoch{ std::move(LastCardReviewAtMillisecondsSinceEpoch) } {
    }

    ~ReviewSessionListRow() = default;
    ReviewSessionListRow(const ReviewSessionListRow&) = delete;
    ReviewSessionListRow(ReviewSessionListRow&&) noexcept = default;
    ReviewSessionListRow& operator=(const ReviewSessionListRow&) = delete;
    ReviewSessionListRow& operator=(ReviewSessionListRow&&) noexcept = default;

    std::string m_ReviewSessionId;
    std::string m_ReviewSessionName;
    std::int64_t m_CreatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_UpdatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_LastCardReviewAtMillisecondsSinceEpoch;
};

}
