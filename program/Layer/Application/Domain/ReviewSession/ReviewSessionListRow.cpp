// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Application/Domain/ReviewSession/ReviewSessionListRow.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Application::Domain::ReviewSession {

ReviewSessionListRow::ReviewSessionListRow(std::string&& ReviewSessionId,
                                           std::string&& ReviewSessionName,
                                           const std::int64_t CreatedAtMillisecondsSinceEpoch,
                                           const std::optional<std::int64_t>& LastUpdatedAtMillisecondsSinceEpochOptional,
                                           const std::optional<std::int64_t>& LastCardReviewAtMillisecondsSinceEpochOptional)
    : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
    , m_ReviewSessionId{ std::move(ReviewSessionId) }
    , m_ReviewSessionName{ std::move(ReviewSessionName) }
    , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
    , m_LastUpdatedAtMillisecondsSinceEpochOptional{ LastUpdatedAtMillisecondsSinceEpochOptional }
    , m_LastCardReviewAtMillisecondsSinceEpochOptional{ LastCardReviewAtMillisecondsSinceEpochOptional } {
}

}
#endif
