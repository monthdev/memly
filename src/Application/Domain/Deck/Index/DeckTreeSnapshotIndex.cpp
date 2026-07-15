#include "Application/Domain/Deck/Index/DeckTreeSnapshotIndex.hpp"

#include <cassert>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Application::Domain::Deck::Index {

[[nodiscard]] std::size_t DeckTreeSnapshotIndex::GetDeckNodePosition(const std::string& DeckId) const {
    const std::unordered_map<std::string_view, std::size_t>::const_iterator DeckNodePositionByDeckIdIterator{ m_DeckNodePositionByDeckIdUnorderedMap.find(
        DeckId) };
    assert(DeckNodePositionByDeckIdIterator not_eq m_DeckNodePositionByDeckIdUnorderedMap.end());
    return DeckNodePositionByDeckIdIterator->second;
}

[[nodiscard]] std::optional<std::reference_wrapper<const DeckTreeSnapshotIndex::DeckTreeSnapshotNode>>
DeckTreeSnapshotIndex::TryGetDeckTreeSnapshotNode(const std::optional<std::string>& DeckIdOptional) const {
    if (not DeckIdOptional.has_value()) {
        return std::nullopt;
    }
    return std::cref(m_DeckTreeSnapshotNodeVector.at(GetDeckNodePosition(DeckIdOptional.value())));
}

[[nodiscard]] const std::vector<std::size_t>&
DeckTreeSnapshotIndex::GetChildDeckNodePositionVector(const std::optional<std::string>& ParentDeckIdOptional) const {
    if (not ParentDeckIdOptional.has_value()) {
        return m_RootDeckNodePositionVector;
    }
    return m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(GetDeckNodePosition(ParentDeckIdOptional.value()));
}

[[nodiscard]] std::vector<std::string_view> DeckTreeSnapshotIndex::GetSubtreeDeckIds(const std::string& DeckId) const {
    std::vector<std::string_view> SubtreeDeckIdVector{};
    std::vector<std::size_t> PendingDeckNodePositionVector{ GetDeckNodePosition(DeckId) };
    while (not PendingDeckNodePositionVector.empty()) {
        const std::size_t DeckNodePosition{ PendingDeckNodePositionVector.back() };
        PendingDeckNodePositionVector.pop_back();
        SubtreeDeckIdVector.push_back(m_DeckTreeSnapshotNodeVector.at(DeckNodePosition).m_DeckId);
        const std::vector<std::size_t>& ChildDeckNodePositionVector{ m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(DeckNodePosition) };
        for (const std::size_t ChildDeckNodePosition : ChildDeckNodePositionVector) { PendingDeckNodePositionVector.push_back(ChildDeckNodePosition); }
    }
    return SubtreeDeckIdVector;
}

void DeckTreeSnapshotIndex::RefreshFromDeckTreeSnapshotNodes(std::vector<DeckTreeSnapshotNode>&& DeckTreeSnapshotNodeVector) {
    m_DeckTreeSnapshotNodeVector = std::move(DeckTreeSnapshotNodeVector);
    m_RootDeckNodePositionVector.clear();
    m_ChildDeckNodePositionVectorByDeckNodePositionVector.clear();
    m_DeckNodePositionByDeckIdUnorderedMap.clear();
    m_RootDeckNodePositionVector.reserve(m_DeckTreeSnapshotNodeVector.size());
    m_ChildDeckNodePositionVectorByDeckNodePositionVector.resize(m_DeckTreeSnapshotNodeVector.size());
    m_DeckNodePositionByDeckIdUnorderedMap.reserve(m_DeckTreeSnapshotNodeVector.size());
    for (std::size_t DeckNodePosition{ 0 }; DeckNodePosition < m_DeckTreeSnapshotNodeVector.size(); ++DeckNodePosition) {
        m_DeckNodePositionByDeckIdUnorderedMap.emplace(std::string_view{ m_DeckTreeSnapshotNodeVector.at(DeckNodePosition).m_DeckId }, DeckNodePosition);
    }
    for (std::size_t DeckNodePosition{ 0 }; DeckNodePosition < m_DeckTreeSnapshotNodeVector.size(); ++DeckNodePosition) {
        const DeckTreeSnapshotNode& CurrentDeckTreeSnapshotNode{ m_DeckTreeSnapshotNodeVector.at(DeckNodePosition) };
        if (not CurrentDeckTreeSnapshotNode.m_ParentDeckIdOptional.has_value()) {
            m_RootDeckNodePositionVector.push_back(DeckNodePosition);
            continue;
        }
        const std::size_t ParentDeckNodePosition{ GetDeckNodePosition(CurrentDeckTreeSnapshotNode.m_ParentDeckIdOptional.value()) };
        m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(ParentDeckNodePosition).push_back(DeckNodePosition);
    }
    for (const std::size_t RootDeckNodePosition : m_RootDeckNodePositionVector) { AccumulateSubtreeCounts(RootDeckNodePosition); }
}

void DeckTreeSnapshotIndex::AccumulateSubtreeCounts(const std::size_t DeckNodePosition) {
    DeckTreeSnapshotNode& CurrentDeckTreeSnapshotNode{ m_DeckTreeSnapshotNodeVector.at(DeckNodePosition) };
    const std::vector<std::size_t>& ChildDeckNodePositionVector{ m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(DeckNodePosition) };
    for (const std::size_t ChildDeckNodePosition : ChildDeckNodePositionVector) {
        AccumulateSubtreeCounts(ChildDeckNodePosition);
        const DeckTreeSnapshotNode& ChildDeckTreeSnapshotNode{ m_DeckTreeSnapshotNodeVector.at(ChildDeckNodePosition) };
        CurrentDeckTreeSnapshotNode.m_SubtreeDueNowCount += ChildDeckTreeSnapshotNode.m_SubtreeDueNowCount;
        CurrentDeckTreeSnapshotNode.m_SubtreeByTodayCount += ChildDeckTreeSnapshotNode.m_SubtreeByTodayCount;
        CurrentDeckTreeSnapshotNode.m_SubtreeTotalCount += ChildDeckTreeSnapshotNode.m_SubtreeTotalCount;
    }
}

[[nodiscard]] bool DeckTreeSnapshotIndex::DoesDuplicateSiblingDeckNameExist(const std::optional<std::string>& ParentDeckIdOptional,
                                                                            const std::string& DeckName) const {
    const std::vector<std::size_t>& SiblingDeckNodePositionVector{ GetChildDeckNodePositionVector(ParentDeckIdOptional) };
    for (const std::size_t DeckNodePosition : SiblingDeckNodePositionVector) {
        if (m_DeckTreeSnapshotNodeVector.at(DeckNodePosition).m_DeckName == DeckName) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] bool DeckTreeSnapshotIndex::WouldMoveDeckBeNoOp(const std::string& MovingDeckId,
                                                              const std::optional<std::string>& NewParentDeckIdOptional) const {
    return m_DeckTreeSnapshotNodeVector.at(GetDeckNodePosition(MovingDeckId)).m_ParentDeckIdOptional == NewParentDeckIdOptional;
}

[[nodiscard]] bool DeckTreeSnapshotIndex::WouldMoveDeckCreateDeckTreeCycle(const std::string& MovingDeckId,
                                                                           const std::optional<std::string>& NewParentDeckIdOptional) const {
    std::optional<std::reference_wrapper<const DeckTreeSnapshotNode>> CurrentDeckNodeOptional{ TryGetDeckTreeSnapshotNode(NewParentDeckIdOptional) };
    while (CurrentDeckNodeOptional.has_value()) {
        const DeckTreeSnapshotNode& CurrentDeckNode{ CurrentDeckNodeOptional.value().get() };
        if (CurrentDeckNode.m_DeckId == MovingDeckId) {
            return true;
        }
        CurrentDeckNodeOptional = TryGetDeckTreeSnapshotNode(CurrentDeckNode.m_ParentDeckIdOptional);
    }
    return false;
}

[[nodiscard]] bool DeckTreeSnapshotIndex::WouldMoveDeckCreateTargetLanguageMismatch(const std::string& DeckId,
                                                                                    const std::optional<std::string>& NewParentDeckIdOptional) const {
    if (not NewParentDeckIdOptional.has_value()) {
        return false;
    }
    return m_DeckTreeSnapshotNodeVector.at(GetDeckNodePosition(DeckId)).m_TargetLanguageCode not_eq
           m_DeckTreeSnapshotNodeVector.at(GetDeckNodePosition(NewParentDeckIdOptional.value())).m_TargetLanguageCode;
}

}
