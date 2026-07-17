#include "Application/Domain/Deck/Index/DeckForestSnapshotIndex.hpp"

#include <cassert>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Application::Domain::Deck::Index {

[[nodiscard]] std::size_t DeckForestSnapshotIndex::GetDeckNodePosition(const std::string& DeckId) const {
    const std::unordered_map<std::string_view, std::size_t>::const_iterator DeckNodePositionByDeckIdIterator{ m_DeckNodePositionByDeckIdUnorderedMap.find(
        DeckId) };
    assert(DeckNodePositionByDeckIdIterator not_eq m_DeckNodePositionByDeckIdUnorderedMap.end());
    return DeckNodePositionByDeckIdIterator->second;
}

[[nodiscard]] std::vector<std::string_view> DeckForestSnapshotIndex::GetSubtreeDeckIds(const std::string& DeckId) const {
    std::vector<std::string_view> SubtreeDeckIdVector{};
    std::vector<std::size_t> PendingDeckNodePositionVector{ GetDeckNodePosition(DeckId) };
    while (not PendingDeckNodePositionVector.empty()) {
        const std::size_t DeckNodePosition{ PendingDeckNodePositionVector.back() };
        PendingDeckNodePositionVector.pop_back();
        SubtreeDeckIdVector.push_back(m_DeckForestSnapshotNodeVector.at(DeckNodePosition).m_DeckId);
        const std::vector<std::size_t>& ChildDeckNodePositionVector{ m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(DeckNodePosition) };
        for (const std::size_t ChildDeckNodePosition : ChildDeckNodePositionVector) { PendingDeckNodePositionVector.push_back(ChildDeckNodePosition); }
    }
    return SubtreeDeckIdVector;
}

void DeckForestSnapshotIndex::RefreshFromDeckForestSnapshotNodes(std::vector<DeckForestSnapshotNode>&& DeckForestSnapshotNodeVector) {
    m_DeckForestSnapshotNodeVector = std::move(DeckForestSnapshotNodeVector);
    m_RootDeckNodePositionVector.clear();
    m_ChildDeckNodePositionVectorByDeckNodePositionVector.clear();
    m_DeckNodePositionByDeckIdUnorderedMap.clear();
    m_RootDeckNodePositionVector.reserve(m_DeckForestSnapshotNodeVector.size());
    m_ChildDeckNodePositionVectorByDeckNodePositionVector.resize(m_DeckForestSnapshotNodeVector.size());
    m_DeckNodePositionByDeckIdUnorderedMap.reserve(m_DeckForestSnapshotNodeVector.size());
    for (std::size_t DeckNodePosition{ 0 }; DeckNodePosition < m_DeckForestSnapshotNodeVector.size(); ++DeckNodePosition) {
        m_DeckNodePositionByDeckIdUnorderedMap.emplace(std::string_view{ m_DeckForestSnapshotNodeVector.at(DeckNodePosition).m_DeckId }, DeckNodePosition);
    }
    for (std::size_t DeckNodePosition{ 0 }; DeckNodePosition < m_DeckForestSnapshotNodeVector.size(); ++DeckNodePosition) {
        const DeckForestSnapshotNode& CurrentDeckForestSnapshotNode{ m_DeckForestSnapshotNodeVector.at(DeckNodePosition) };
        if (not CurrentDeckForestSnapshotNode.m_ParentDeckIdOptional.has_value()) {
            m_RootDeckNodePositionVector.push_back(DeckNodePosition);
            continue;
        }
        /// \note `GetDeckNodePosition()` asserts that the parent deck ID exists while constructing the adjacency list.
        const std::size_t ParentDeckNodePosition{ GetDeckNodePosition(CurrentDeckForestSnapshotNode.m_ParentDeckIdOptional.value()) };
        m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(ParentDeckNodePosition).push_back(DeckNodePosition);
    }
    AccumulateSubtreeCounts();
}

void DeckForestSnapshotIndex::AccumulateSubtreeCounts() {
    std::vector<std::size_t> ParentBeforeChildDeckNodePositionVector{ m_RootDeckNodePositionVector };
    ParentBeforeChildDeckNodePositionVector.reserve(m_DeckForestSnapshotNodeVector.size());
    for (std::size_t DeckNodePositionIndex{ 0 }; DeckNodePositionIndex not_eq ParentBeforeChildDeckNodePositionVector.size(); ++DeckNodePositionIndex) {
        ParentBeforeChildDeckNodePositionVector.append_range(
            m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(ParentBeforeChildDeckNodePositionVector.at(DeckNodePositionIndex)));
    }
    /// \note Reachability from roots detects cycles under the one-parent invariant.
    assert(ParentBeforeChildDeckNodePositionVector.size() == m_DeckForestSnapshotNodeVector.size());
    for (std::size_t RemainingDeckNodePositionCount{ ParentBeforeChildDeckNodePositionVector.size() }; RemainingDeckNodePositionCount > 0;
         --RemainingDeckNodePositionCount) {
        const std::size_t DeckNodePosition{ ParentBeforeChildDeckNodePositionVector.at(RemainingDeckNodePositionCount - 1) };
        DeckForestSnapshotNode& CurrentDeckForestSnapshotNode{ m_DeckForestSnapshotNodeVector.at(DeckNodePosition) };
        const std::vector<std::size_t>& ChildDeckNodePositionVector{ m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(DeckNodePosition) };
        for (const std::size_t ChildDeckNodePosition : ChildDeckNodePositionVector) {
            const DeckForestSnapshotNode& ChildDeckForestSnapshotNode{ m_DeckForestSnapshotNodeVector.at(ChildDeckNodePosition) };
            CurrentDeckForestSnapshotNode.m_SubtreeDueNowCount += ChildDeckForestSnapshotNode.m_SubtreeDueNowCount;
            CurrentDeckForestSnapshotNode.m_SubtreeByTodayCount += ChildDeckForestSnapshotNode.m_SubtreeByTodayCount;
            CurrentDeckForestSnapshotNode.m_SubtreeTotalCount += ChildDeckForestSnapshotNode.m_SubtreeTotalCount;
        }
    }
}

[[nodiscard]] bool DeckForestSnapshotIndex::DoesDuplicateSiblingDeckNameExist(const std::optional<std::string>& ParentDeckIdOptional,
                                                                              const std::string& DeckName) const {
    for (const std::size_t DeckNodePosition : ParentDeckIdOptional.has_value() ?
                                                  m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(GetDeckNodePosition(ParentDeckIdOptional.value())) :
                                                  m_RootDeckNodePositionVector) {
        if (m_DeckForestSnapshotNodeVector.at(DeckNodePosition).m_DeckName == DeckName) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] bool DeckForestSnapshotIndex::WouldMoveDeckBeNoOp(const std::string& MovingDeckId,
                                                                const std::optional<std::string>& NewParentDeckIdOptional) const {
    return m_DeckForestSnapshotNodeVector.at(GetDeckNodePosition(MovingDeckId)).m_ParentDeckIdOptional == NewParentDeckIdOptional;
}

[[nodiscard]] bool DeckForestSnapshotIndex::WouldMoveDeckCreateCycle(const std::string& MovingDeckId,
                                                                     const std::optional<std::string>& NewParentDeckIdOptional) const {
    if (not NewParentDeckIdOptional.has_value()) {
        return false;
    }
    std::size_t CurrentDeckNodePosition{ GetDeckNodePosition(NewParentDeckIdOptional.value()) };
    while (true) {
        const DeckForestSnapshotNode& CurrentDeckNode{ m_DeckForestSnapshotNodeVector.at(CurrentDeckNodePosition) };
        if (CurrentDeckNode.m_DeckId == MovingDeckId) {
            return true;
        }
        if (not CurrentDeckNode.m_ParentDeckIdOptional.has_value()) {
            return false;
        }
        CurrentDeckNodePosition = GetDeckNodePosition(CurrentDeckNode.m_ParentDeckIdOptional.value());
    }
}

[[nodiscard]] bool DeckForestSnapshotIndex::WouldMoveDeckCreateTargetLanguageMismatch(const std::string& DeckId,
                                                                                      const std::optional<std::string>& NewParentDeckIdOptional) const {
    if (not NewParentDeckIdOptional.has_value()) {
        return false;
    }
    return m_DeckForestSnapshotNodeVector.at(GetDeckNodePosition(DeckId)).m_TargetLanguageCode not_eq
           m_DeckForestSnapshotNodeVector.at(GetDeckNodePosition(NewParentDeckIdOptional.value())).m_TargetLanguageCode;
}

}
