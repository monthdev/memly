#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Application::Domain::ReviewSession {

struct ReviewSessionListRow : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
    std::string m_ReviewSessionId;
    std::string m_ReviewSessionName;
    std::int64_t m_CreatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_LastUpdatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_LastCardReviewAtMillisecondsSinceEpoch;

    explicit ReviewSessionListRow(std::string&& ReviewSessionId,
                                  std::string&& ReviewSessionName,
                                  const std::int64_t CreatedAtMillisecondsSinceEpoch,
                                  const std::optional<std::int64_t>& LastUpdatedAtMillisecondsSinceEpoch,
                                  const std::optional<std::int64_t>& LastCardReviewAtMillisecondsSinceEpoch)
        : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
        , m_ReviewSessionId{ std::move(ReviewSessionId) }
        , m_ReviewSessionName{ std::move(ReviewSessionName) }
        , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
        , m_LastUpdatedAtMillisecondsSinceEpoch{ LastUpdatedAtMillisecondsSinceEpoch }
        , m_LastCardReviewAtMillisecondsSinceEpoch{ LastCardReviewAtMillisecondsSinceEpoch } {
    }
};

}
