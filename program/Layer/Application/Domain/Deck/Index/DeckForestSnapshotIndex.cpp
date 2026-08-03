#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotIndex.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"

namespace Layer::Application::Domain::Deck::Index {

[[nodiscard]] auto DeckForestSnapshotIndex::GetDeckNodePosition(const std::string& DeckId) const -> std::size_t {
    const std::unordered_map<std::string_view, std::size_t>::const_iterator DeckNodePositionByDeckIdIterator{ this->m_DeckNodePositionByDeckIdUnorderedMap.find(
        DeckId) };
    assert(DeckNodePositionByDeckIdIterator not_eq this->m_DeckNodePositionByDeckIdUnorderedMap.end());
    return std::size_t{ DeckNodePositionByDeckIdIterator->second };
}

[[nodiscard]] auto DeckForestSnapshotIndex::GetSubtreeDeckIds(const std::string& DeckId) const -> std::vector<std::string_view> {
    std::vector<std::string_view> SubtreeDeckIdVector{};
    std::vector<std::size_t> PendingDeckNodePositionVector{ this->GetDeckNodePosition(DeckId) };
    while (not PendingDeckNodePositionVector.empty()) {
        const std::size_t DeckNodePosition{ PendingDeckNodePositionVector.back() };
        PendingDeckNodePositionVector.pop_back();
        SubtreeDeckIdVector.push_back(this->m_DeckForestSnapshotNodeVector.at(DeckNodePosition).m_DeckId);
        const std::vector<std::size_t>& ChildDeckNodePositionVector{ this->m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(DeckNodePosition) };
        for (const std::size_t ChildDeckNodePosition : ChildDeckNodePositionVector) { PendingDeckNodePositionVector.push_back(ChildDeckNodePosition); }
    }
    return std::vector<std::string_view>{ std::move(SubtreeDeckIdVector) };
}

void DeckForestSnapshotIndex::RefreshFromDeckForestSnapshotNodes(std::vector<DeckForestSnapshotNode>&& DeckForestSnapshotNodeVector) {
    this->m_DeckForestSnapshotNodeVector = std::move(DeckForestSnapshotNodeVector);
    this->m_RootDeckNodePositionVector.clear();
    this->m_ChildDeckNodePositionVectorByDeckNodePositionVector.clear();
    this->m_DeckNodePositionByDeckIdUnorderedMap.clear();
    this->m_RootDeckNodePositionVector.reserve(this->m_DeckForestSnapshotNodeVector.size());
    this->m_ChildDeckNodePositionVectorByDeckNodePositionVector.resize(this->m_DeckForestSnapshotNodeVector.size());
    this->m_DeckNodePositionByDeckIdUnorderedMap.reserve(this->m_DeckForestSnapshotNodeVector.size());
    for (std::size_t DeckNodePosition{ 0 }; DeckNodePosition < this->m_DeckForestSnapshotNodeVector.size(); ++DeckNodePosition) {
        this->m_DeckNodePositionByDeckIdUnorderedMap.emplace(std::string_view{ this->m_DeckForestSnapshotNodeVector.at(DeckNodePosition).m_DeckId },
                                                             DeckNodePosition);
    }
    for (std::size_t DeckNodePosition{ 0 }; DeckNodePosition < this->m_DeckForestSnapshotNodeVector.size(); ++DeckNodePosition) {
        const DeckForestSnapshotNode& CurrentDeckForestSnapshotNode{ this->m_DeckForestSnapshotNodeVector.at(DeckNodePosition) };
        if (not CurrentDeckForestSnapshotNode.m_ParentDeckIdOptional.has_value()) {
            this->m_RootDeckNodePositionVector.push_back(DeckNodePosition);
            continue;
        }
        const std::size_t ParentDeckNodePosition{ this->GetDeckNodePosition(CurrentDeckForestSnapshotNode.m_ParentDeckIdOptional.value()) };
        this->m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(ParentDeckNodePosition).push_back(DeckNodePosition);
    }
    this->AccumulateSubtreeCounts();
}

void DeckForestSnapshotIndex::AccumulateSubtreeCounts() {
    std::vector<std::size_t> ParentBeforeChildDeckNodePositionVector{ this->m_RootDeckNodePositionVector };
    ParentBeforeChildDeckNodePositionVector.reserve(this->m_DeckForestSnapshotNodeVector.size());
    for (std::size_t DeckNodePositionIndex{ 0 }; DeckNodePositionIndex not_eq ParentBeforeChildDeckNodePositionVector.size(); ++DeckNodePositionIndex) {
        ParentBeforeChildDeckNodePositionVector.append_range(
            this->m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(ParentBeforeChildDeckNodePositionVector.at(DeckNodePositionIndex)));
    }
    assert(ParentBeforeChildDeckNodePositionVector.size() == this->m_DeckForestSnapshotNodeVector.size());
    for (std::size_t RemainingDeckNodePositionCount{ ParentBeforeChildDeckNodePositionVector.size() }; RemainingDeckNodePositionCount > 0;
         --RemainingDeckNodePositionCount) {
        const std::size_t DeckNodePosition{ ParentBeforeChildDeckNodePositionVector.at(RemainingDeckNodePositionCount - 1) };
        DeckForestSnapshotNode& CurrentDeckForestSnapshotNode{ this->m_DeckForestSnapshotNodeVector.at(DeckNodePosition) };
        const std::vector<std::size_t>& ChildDeckNodePositionVector{ this->m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(DeckNodePosition) };
        for (const std::size_t ChildDeckNodePosition : ChildDeckNodePositionVector) {
            const DeckForestSnapshotNode& ChildDeckForestSnapshotNode{ this->m_DeckForestSnapshotNodeVector.at(ChildDeckNodePosition) };
            CurrentDeckForestSnapshotNode.m_SubtreeDueNowCount += ChildDeckForestSnapshotNode.m_SubtreeDueNowCount;
            CurrentDeckForestSnapshotNode.m_SubtreeByTodayCount += ChildDeckForestSnapshotNode.m_SubtreeByTodayCount;
            CurrentDeckForestSnapshotNode.m_SubtreeTotalCount += ChildDeckForestSnapshotNode.m_SubtreeTotalCount;
        }
    }
}

[[nodiscard]] auto DeckForestSnapshotIndex::DoesDuplicateSiblingDeckNameExist(const std::optional<std::string>& ParentDeckIdOptional,
                                                                              const std::string& DeckName) const -> bool {
    return bool{ std::ranges::any_of(ParentDeckIdOptional.has_value() ? this->m_ChildDeckNodePositionVectorByDeckNodePositionVector.at(
                                                                            this->GetDeckNodePosition(ParentDeckIdOptional.value())) :
                                                                        this->m_RootDeckNodePositionVector,
                                     [this, &DeckName](const std::size_t DeckNodePosition) -> bool {
                                         return bool{ this->m_DeckForestSnapshotNodeVector.at(DeckNodePosition).m_DeckName == DeckName };
                                     }) };
}

[[nodiscard]] auto DeckForestSnapshotIndex::WouldMoveDeckBeNoOp(const std::string& MovingDeckId,
                                                                const std::optional<std::string>& NewParentDeckIdOptional) const -> bool {
    return bool{ this->m_DeckForestSnapshotNodeVector.at(this->GetDeckNodePosition(MovingDeckId)).m_ParentDeckIdOptional == NewParentDeckIdOptional };
}

[[nodiscard]] auto DeckForestSnapshotIndex::WouldMoveDeckCreateCycle(const std::string& MovingDeckId,
                                                                     const std::optional<std::string>& NewParentDeckIdOptional) const -> bool {
    if (not NewParentDeckIdOptional.has_value()) {
        return bool{ false };
    }
    std::size_t CurrentDeckNodePosition{ this->GetDeckNodePosition(NewParentDeckIdOptional.value()) };
    while (true) {
        const DeckForestSnapshotNode& CurrentDeckNode{ this->m_DeckForestSnapshotNodeVector.at(CurrentDeckNodePosition) };
        if (CurrentDeckNode.m_DeckId == MovingDeckId) {
            return bool{ true };
        }
        if (not CurrentDeckNode.m_ParentDeckIdOptional.has_value()) {
            return bool{ false };
        }
        CurrentDeckNodePosition = this->GetDeckNodePosition(CurrentDeckNode.m_ParentDeckIdOptional.value());
    }
}

[[nodiscard]] auto DeckForestSnapshotIndex::WouldMoveDeckCreateTargetLanguageMismatch(const std::string& DeckId,
                                                                                      const std::optional<std::string>& NewParentDeckIdOptional) const -> bool {
    if (not NewParentDeckIdOptional.has_value()) {
        return bool{ false };
    }
    return bool{ this->m_DeckForestSnapshotNodeVector.at(this->GetDeckNodePosition(DeckId)).m_TargetLanguageCode not_eq
                 this->m_DeckForestSnapshotNodeVector.at(this->GetDeckNodePosition(NewParentDeckIdOptional.value())).m_TargetLanguageCode };
}

}
