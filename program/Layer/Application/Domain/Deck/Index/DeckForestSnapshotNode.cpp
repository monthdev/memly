#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Application::Domain::Deck::Index {

DeckForestSnapshotNode::DeckForestSnapshotNode(std::string&& DeckId,
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

}
