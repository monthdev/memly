#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace Domain::Deck {

struct DeckTreeSnapshotNode {
    DeckTreeSnapshotNode(std::string&& DeckId,
                         std::optional<std::string>&& ParentDeckId,
                         std::string&& DeckName,
                         const std::int64_t CreatedAtMillisecondsSinceEpoch,
                         std::optional<std::int64_t>&& LastUpdatedAtMillisecondsSinceEpoch,
                         const std::uint32_t SelfDueNowCount,
                         const std::uint32_t SelfByTodayCount,
                         const std::uint32_t SelfTotalCount,
                         const std::uint32_t SubtreeDueNowCount,
                         const std::uint32_t SubtreeByTodayCount,
                         const std::uint32_t SubtreeTotalCount,
                         const std::uint8_t TargetLanguageCode)
        : m_DeckId{ std::move(DeckId) }
        , m_ParentDeckId{ std::move(ParentDeckId) }
        , m_DeckName{ std::move(DeckName) }
        , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
        , m_LastUpdatedAtMillisecondsSinceEpoch{ std::move(LastUpdatedAtMillisecondsSinceEpoch) }
        , m_SelfDueNowCount{ SelfDueNowCount }
        , m_SelfByTodayCount{ SelfByTodayCount }
        , m_SelfTotalCount{ SelfTotalCount }
        , m_SubtreeDueNowCount{ SubtreeDueNowCount }
        , m_SubtreeByTodayCount{ SubtreeByTodayCount }
        , m_SubtreeTotalCount{ SubtreeTotalCount }
        , m_TargetLanguageCode{ TargetLanguageCode } {
    }

    ~DeckTreeSnapshotNode() noexcept = default;
    DeckTreeSnapshotNode(const DeckTreeSnapshotNode&) = delete;
    DeckTreeSnapshotNode(DeckTreeSnapshotNode&&) noexcept = default;
    DeckTreeSnapshotNode& operator=(const DeckTreeSnapshotNode&) = delete;
    DeckTreeSnapshotNode& operator=(DeckTreeSnapshotNode&&) noexcept = default;

    std::string m_DeckId;
    std::optional<std::string> m_ParentDeckId;
    std::string m_DeckName;
    std::int64_t m_CreatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_LastUpdatedAtMillisecondsSinceEpoch;
    std::uint32_t m_SelfDueNowCount;
    std::uint32_t m_SelfByTodayCount;
    std::uint32_t m_SelfTotalCount;
    std::uint32_t m_SubtreeDueNowCount;
    std::uint32_t m_SubtreeByTodayCount;
    std::uint32_t m_SubtreeTotalCount;
    std::uint8_t m_TargetLanguageCode;
};

}
