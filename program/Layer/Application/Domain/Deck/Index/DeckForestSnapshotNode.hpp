#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Application::Domain::Deck::Index {

struct DeckForestSnapshotNode final : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
    std::string m_DeckId;
    std::optional<std::string> m_ParentDeckIdOptional;
    std::string m_DeckName;
    std::int64_t m_CreatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_LastUpdatedAtMillisecondsSinceEpochOptional;
    std::uint32_t m_SelfDueNowCount;
    std::uint32_t m_SelfByTodayCount;
    std::uint32_t m_SelfTotalCount;
    std::uint8_t m_TargetLanguageCode;
    std::uint32_t m_SubtreeDueNowCount;
    std::uint32_t m_SubtreeByTodayCount;
    std::uint32_t m_SubtreeTotalCount;

    explicit DeckForestSnapshotNode(std::string&&,
                                    std::optional<std::string>&&,
                                    std::string&&,
                                    std::int64_t,
                                    const std::optional<std::int64_t>&,
                                    std::uint32_t,
                                    std::uint32_t,
                                    std::uint32_t,
                                    std::uint8_t);
};

}
