// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Application::Domain::ReviewSession {

struct [[nodiscard]] ReviewSessionListRow final : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
    std::string m_ReviewSessionId;
    std::string m_ReviewSessionName;
    std::int64_t m_CreatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_LastUpdatedAtMillisecondsSinceEpochOptional;
    std::optional<std::int64_t> m_LastCardReviewAtMillisecondsSinceEpochOptional;

    explicit ReviewSessionListRow(std::string&&,
                                  std::string&&,
                                  std::int64_t,
                                  const std::optional<std::int64_t>&,
                                  const std::optional<std::int64_t>&);
};

}
#endif
