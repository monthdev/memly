module;

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

module Memly.Repository.DeckSnapshotRecord;

import Memly.Database.DecodableQueryResultRowMixin;

namespace Memly::Repository {

DeckSnapshotRecord::DeckSnapshotRecord(
    std::string&& DeckId,
    std::optional<std::string>&& ParentDeckIdOptional,
    std::string&& DeckName,
    const std::int64_t CreatedAtMillisecondsSinceEpoch,
    const std::optional<std::int64_t>&
        LastUpdatedAtMillisecondsSinceEpochOptional,
    const std::uint32_t SelfDueNowCount,
    const std::uint32_t SelfByTodayCount,
    const std::uint32_t SelfTotalCount,
    const std::uint8_t TargetLanguageCode
) noexcept :
    DecodableQueryResultRowMixin{}, m_DeckId{ std::move(DeckId) },
    m_ParentDeckIdOptional{ std::move(ParentDeckIdOptional) },
    m_DeckName{ std::move(DeckName) },
    m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch },
    m_LastUpdatedAtMillisecondsSinceEpochOptional{
        LastUpdatedAtMillisecondsSinceEpochOptional
    },
    m_SelfDueNowCount{ SelfDueNowCount },
    m_SelfByTodayCount{ SelfByTodayCount }, m_SelfTotalCount{ SelfTotalCount },
    m_TargetLanguageCode{ TargetLanguageCode } {
}

}
