#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace Infrastructure::Store::Deck {

struct DeckSnapshotRecord final {
    std::string m_DeckId;
    std::optional<std::string> m_ParentDeckIdOptional;
    std::string m_DeckName;
    std::int64_t m_CreatedAtMillisecondsSinceEpoch;
    std::optional<std::int64_t> m_LastUpdatedAtMillisecondsSinceEpochOptional;
    std::uint32_t m_SelfDueNowCount;
    std::uint32_t m_SelfByTodayCount;
    std::uint32_t m_SelfTotalCount;
    std::uint8_t m_TargetLanguageCode;

    explicit DeckSnapshotRecord(std::string&& DeckId,
                                std::optional<std::string>&& ParentDeckIdOptional,
                                std::string&& DeckName,
                                const std::int64_t CreatedAtMillisecondsSinceEpoch,
                                const std::optional<std::int64_t>& LastUpdatedAtMillisecondsSinceEpochOptional,
                                const std::uint32_t SelfDueNowCount,
                                const std::uint32_t SelfByTodayCount,
                                const std::uint32_t SelfTotalCount,
                                const std::uint8_t TargetLanguageCode)
        : m_DeckId{ std::move(DeckId) }
        , m_ParentDeckIdOptional{ std::move(ParentDeckIdOptional) }
        , m_DeckName{ std::move(DeckName) }
        , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
        , m_LastUpdatedAtMillisecondsSinceEpochOptional{ LastUpdatedAtMillisecondsSinceEpochOptional }
        , m_SelfDueNowCount{ SelfDueNowCount }
        , m_SelfByTodayCount{ SelfByTodayCount }
        , m_SelfTotalCount{ SelfTotalCount }
        , m_TargetLanguageCode{ TargetLanguageCode } {
    }

    explicit DeckSnapshotRecord(const DeckSnapshotRecord&) = delete;
    explicit DeckSnapshotRecord(DeckSnapshotRecord&&) noexcept = default;
    auto operator=(const DeckSnapshotRecord&) -> DeckSnapshotRecord& = delete;
    auto operator=(DeckSnapshotRecord&&) -> DeckSnapshotRecord& = delete;
};

}
