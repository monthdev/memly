module;

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

export module Memly.Domain.DeckForestSnapshotIndex;

import Memly.Domain.DeckForestSnapshotNode;

export namespace Memly::Domain {

class DeckForestSnapshotIndex final {
private:
    std::vector<DeckForestSnapshotNode> m_DeckForestSnapshotNodeVector;
    std::vector<std::size_t> m_RootDeckNodePositionVector;
    std::vector<std::vector<std::size_t>>
        m_ChildDeckNodePositionVectorByDeckNodePositionVector;
    std::unordered_map<std::string_view, std::size_t>
        m_DeckNodePositionByDeckIdUnorderedMap;

public:
    explicit DeckForestSnapshotIndex() noexcept;

    explicit DeckForestSnapshotIndex(const DeckForestSnapshotIndex&) = delete;
    DeckForestSnapshotIndex&
    operator=(const DeckForestSnapshotIndex&) = delete;

    explicit DeckForestSnapshotIndex(DeckForestSnapshotIndex&&) = delete;
    DeckForestSnapshotIndex&
    operator=(DeckForestSnapshotIndex&&) = delete;

    ~DeckForestSnapshotIndex() noexcept = default;

    [[nodiscard]] std::vector<std::string_view>
    GetSubtreeDeckIds(const std::string&) const;

    [[nodiscard]] bool
    DoesDuplicateSiblingDeckNameExist(
        const std::optional<std::string>&,
        const std::string&
    ) const;

    [[nodiscard]] bool
    WouldMoveDeckBeNoOp(
        const std::string&,
        const std::optional<std::string>&
    ) const;

    [[nodiscard]] bool
    WouldMoveDeckCreateCycle(
        const std::string&,
        const std::optional<std::string>&
    ) const;

    [[nodiscard]] bool
    WouldMoveDeckCreateTargetLanguageMismatch(
        const std::string&,
        const std::optional<std::string>&
    ) const;

    void
    RefreshFromDeckForestSnapshotNodes(std::vector<DeckForestSnapshotNode>&&);

private:
    [[nodiscard]] std::size_t
    GetDeckNodePosition(const std::string&) const;

    void
    AccumulateSubtreeCounts();
};

}
