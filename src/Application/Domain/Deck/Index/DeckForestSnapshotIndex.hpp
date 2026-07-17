#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Application::Domain::Deck::Index {

class DeckForestSnapshotIndex final {
public:
    struct DeckForestSnapshotNode final {
        DeckForestSnapshotNode(std::string&& DeckId,
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
            , m_TargetLanguageCode{ TargetLanguageCode }
            , m_SubtreeDueNowCount{ SelfDueNowCount }
            , m_SubtreeByTodayCount{ SelfByTodayCount }
            , m_SubtreeTotalCount{ SelfTotalCount } {
        }

        DeckForestSnapshotNode(const DeckForestSnapshotNode&) = delete;
        DeckForestSnapshotNode(DeckForestSnapshotNode&&) noexcept = default;
        DeckForestSnapshotNode& operator=(const DeckForestSnapshotNode&) = delete;
        DeckForestSnapshotNode& operator=(DeckForestSnapshotNode&&) noexcept = default;

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
    };

    DeckForestSnapshotIndex() noexcept
        : m_DeckForestSnapshotNodeVector{}
        , m_RootDeckNodePositionVector{}
        , m_ChildDeckNodePositionVectorByDeckNodePositionVector{}
        , m_DeckNodePositionByDeckIdUnorderedMap{} {
    }

    DeckForestSnapshotIndex(const DeckForestSnapshotIndex&) = delete;
    DeckForestSnapshotIndex(DeckForestSnapshotIndex&&) = delete;
    DeckForestSnapshotIndex& operator=(const DeckForestSnapshotIndex&) = delete;
    DeckForestSnapshotIndex& operator=(DeckForestSnapshotIndex&&) = delete;

    [[nodiscard]] std::optional<std::reference_wrapper<const DeckForestSnapshotNode>> TryGetDeckForestSnapshotNode(const std::optional<std::string>&) const;

    [[nodiscard]] std::vector<std::string_view> GetSubtreeDeckIds(const std::string&) const;

    [[nodiscard]] bool DoesDuplicateSiblingDeckNameExist(const std::optional<std::string>&, const std::string&) const;
    [[nodiscard]] bool WouldMoveDeckBeNoOp(const std::string&, const std::optional<std::string>&) const;
    [[nodiscard]] bool WouldMoveDeckCreateCycle(const std::string&, const std::optional<std::string>&) const;
    [[nodiscard]] bool WouldMoveDeckCreateTargetLanguageMismatch(const std::string&, const std::optional<std::string>&) const;

    void RefreshFromDeckForestSnapshotNodes(std::vector<DeckForestSnapshotNode>&&);

private:
    std::vector<DeckForestSnapshotNode> m_DeckForestSnapshotNodeVector;
    std::vector<std::size_t> m_RootDeckNodePositionVector;
    std::vector<std::vector<std::size_t>> m_ChildDeckNodePositionVectorByDeckNodePositionVector;
    std::unordered_map<std::string_view, std::size_t> m_DeckNodePositionByDeckIdUnorderedMap;

    [[nodiscard]] std::size_t GetDeckNodePosition(const std::string&) const;
    [[nodiscard]] const std::vector<std::size_t>& GetChildDeckNodePositionVector(const std::optional<std::string>&) const;

    void AccumulateSubtreeCounts();
};

}
