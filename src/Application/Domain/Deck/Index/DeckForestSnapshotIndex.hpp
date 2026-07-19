#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"

namespace Application::Domain::Deck::Index {

class DeckForestSnapshotIndex final {
private:
    std::vector<DeckForestSnapshotNode> m_DeckForestSnapshotNodeVector;
    std::vector<std::size_t> m_RootDeckNodePositionVector;
    std::vector<std::vector<std::size_t>> m_ChildDeckNodePositionVectorByDeckNodePositionVector;
    std::unordered_map<std::string_view, std::size_t> m_DeckNodePositionByDeckIdUnorderedMap;

public:
    explicit DeckForestSnapshotIndex() noexcept
        : m_DeckForestSnapshotNodeVector{}
        , m_RootDeckNodePositionVector{}
        , m_ChildDeckNodePositionVectorByDeckNodePositionVector{}
        , m_DeckNodePositionByDeckIdUnorderedMap{} {
    }

    explicit DeckForestSnapshotIndex(const DeckForestSnapshotIndex&) = delete;
    explicit DeckForestSnapshotIndex(DeckForestSnapshotIndex&&) = delete;
    auto operator=(const DeckForestSnapshotIndex&) -> DeckForestSnapshotIndex& = delete;
    auto operator=(DeckForestSnapshotIndex&&) -> DeckForestSnapshotIndex& = delete;

    [[nodiscard]] auto GetSubtreeDeckIds(const std::string&) const -> std::vector<std::string_view>;

    [[nodiscard]] auto DoesDuplicateSiblingDeckNameExist(const std::optional<std::string>&, const std::string&) const -> bool;
    [[nodiscard]] auto WouldMoveDeckBeNoOp(const std::string&, const std::optional<std::string>&) const -> bool;
    [[nodiscard]] auto WouldMoveDeckCreateCycle(const std::string&, const std::optional<std::string>&) const -> bool;
    [[nodiscard]] auto WouldMoveDeckCreateTargetLanguageMismatch(const std::string&, const std::optional<std::string>&) const -> bool;

    void RefreshFromDeckForestSnapshotNodes(std::vector<DeckForestSnapshotNode>&&);

private:
    [[nodiscard]] auto GetDeckNodePosition(const std::string&) const -> std::size_t;

    void AccumulateSubtreeCounts();
};

}
