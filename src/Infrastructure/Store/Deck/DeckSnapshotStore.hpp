#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/Deck/Query/DeckQuerySql.hpp"

namespace Infrastructure::Store::Deck {

class DeckSnapshotStore final {
public:
    struct DeckSnapshotRecord final {
        DeckSnapshotRecord(std::string&& DeckId,
                           std::optional<std::string>&& ParentDeckIdOptional,
                           std::string&& DeckName,
                           const std::int64_t CreatedAtMillisecondsSinceEpoch,
                           std::optional<std::int64_t>&& LastUpdatedAtMillisecondsSinceEpochOptional,
                           const std::uint32_t SelfDueNowCount,
                           const std::uint32_t SelfByTodayCount,
                           const std::uint32_t SelfTotalCount,
                           const std::uint8_t TargetLanguageCode)
            : m_DeckId{ std::move(DeckId) }
            , m_ParentDeckIdOptional{ std::move(ParentDeckIdOptional) }
            , m_DeckName{ std::move(DeckName) }
            , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
            , m_LastUpdatedAtMillisecondsSinceEpochOptional{ std::move(LastUpdatedAtMillisecondsSinceEpochOptional) }
            , m_SelfDueNowCount{ SelfDueNowCount }
            , m_SelfByTodayCount{ SelfByTodayCount }
            , m_SelfTotalCount{ SelfTotalCount }
            , m_TargetLanguageCode{ TargetLanguageCode } {
        }
        DeckSnapshotRecord(const DeckSnapshotRecord&) = delete;
        DeckSnapshotRecord(DeckSnapshotRecord&&) noexcept = default;
        DeckSnapshotRecord& operator=(const DeckSnapshotRecord&) = delete;
        DeckSnapshotRecord& operator=(DeckSnapshotRecord&&) noexcept = default;

        std::string m_DeckId;
        std::optional<std::string> m_ParentDeckIdOptional;
        std::string m_DeckName;
        std::int64_t m_CreatedAtMillisecondsSinceEpoch;
        std::optional<std::int64_t> m_LastUpdatedAtMillisecondsSinceEpochOptional;
        std::uint32_t m_SelfDueNowCount;
        std::uint32_t m_SelfByTodayCount;
        std::uint32_t m_SelfTotalCount;
        std::uint8_t m_TargetLanguageCode;
    };

    explicit DeckSnapshotStore(duckdb::Connection& DatabaseConnection)
        : m_ReadDeckSnapshotRecordsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Query::ReadDeckSnapshotRecordsSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadDeckSnapshotRecordsPreparedStatement);
    }
    DeckSnapshotStore(const DeckSnapshotStore&) = delete;
    DeckSnapshotStore(DeckSnapshotStore&&) = delete;
    DeckSnapshotStore& operator=(const DeckSnapshotStore&) = delete;
    DeckSnapshotStore& operator=(DeckSnapshotStore&&) = delete;

    [[nodiscard]] std::vector<DeckSnapshotRecord> ReadDeckSnapshotRecords(std::int64_t);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadDeckSnapshotRecordsPreparedStatement;
};
}
