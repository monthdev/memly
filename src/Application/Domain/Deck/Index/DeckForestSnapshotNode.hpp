#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Application::Domain::Deck::Index {

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

    explicit DeckForestSnapshotNode(std::string&& DeckId,
                                    std::optional<std::string>&& ParentDeckIdOptional,
                                    std::string&& DeckName,
                                    const std::int64_t CreatedAtMillisecondsSinceEpoch,
                                    const std::optional<std::int64_t>& LastUpdatedAtMillisecondsSinceEpochOptional,
                                    const std::uint32_t SelfDueNowCount,
                                    const std::uint32_t SelfByTodayCount,
                                    const std::uint32_t SelfTotalCount,
                                    const std::uint8_t TargetLanguageCode)
        : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
        , m_DeckId{ std::move(DeckId) }
        , m_ParentDeckIdOptional{ std::move(ParentDeckIdOptional) }
        , m_DeckName{ std::move(DeckName) }
        , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
        , m_LastUpdatedAtMillisecondsSinceEpochOptional{ LastUpdatedAtMillisecondsSinceEpochOptional }
        , m_SelfDueNowCount{ SelfDueNowCount }
        , m_SelfByTodayCount{ SelfByTodayCount }
        , m_SelfTotalCount{ SelfTotalCount }
        , m_TargetLanguageCode{ TargetLanguageCode }
        , m_SubtreeDueNowCount{ SelfDueNowCount }
        , m_SubtreeByTodayCount{ SelfByTodayCount }
        , m_SubtreeTotalCount{ SelfTotalCount } {
    }
};

}
