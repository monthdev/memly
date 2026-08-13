// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace Layer::Application::Domain {

struct [[nodiscard]] ReviewSessionListRow final {
    std::string m_ReviewSessionId;
    std::string m_ReviewSessionName;
    std::int64_t m_CreatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_LastUpdatedAtMillisecondsSinceEpochOptional;
    std::optional<std::int64_t> m_LastCardReviewAtMillisecondsSinceEpochOptional;

    explicit ReviewSessionListRow(std::string&& ReviewSessionId,
                                  std::string&& ReviewSessionName,
                                  const std::int64_t CreatedAtMillisecondsSinceEpoch,
                                  const std::optional<std::int64_t>& LastUpdatedAtMillisecondsSinceEpochOptional,
                                  const std::optional<std::int64_t>& LastCardReviewAtMillisecondsSinceEpochOptional) noexcept
        : m_ReviewSessionId{ std::move(ReviewSessionId) }
        , m_ReviewSessionName{ std::move(ReviewSessionName) }
        , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
        , m_LastUpdatedAtMillisecondsSinceEpochOptional{ LastUpdatedAtMillisecondsSinceEpochOptional }
        , m_LastCardReviewAtMillisecondsSinceEpochOptional{ LastCardReviewAtMillisecondsSinceEpochOptional } {
    }

    explicit ReviewSessionListRow(const ReviewSessionListRow&) = delete;
    auto operator=(const ReviewSessionListRow&) -> ReviewSessionListRow& = delete;

    explicit ReviewSessionListRow(ReviewSessionListRow&&) noexcept = default;
    auto operator=(ReviewSessionListRow&&) -> ReviewSessionListRow& = delete;

    ~ReviewSessionListRow() noexcept = default;
};

}
#endif
