#include "Presentation/Model/Deck/DeckTreeSnapshotModel.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "Runtime/Crash.hpp"

namespace Presentation::Model::Deck {

[[nodiscard]] std::optional<std::reference_wrapper<const DeckTreeSnapshotModel::DeckNode>>
DeckTreeSnapshotModel::TryGetDeckNode(const QModelIndex& Index) const noexcept {
    if (not Index.isValid()) {
        return std::nullopt;
    }
    const std::size_t DeckNodeIndex{ static_cast<std::size_t>(Index.internalId()) };
    Q_ASSERT(DeckNodeIndex < m_DeckNodesVector.size());
    return m_DeckNodesVector.at(DeckNodeIndex);
}

[[nodiscard]] const std::vector<std::size_t>& DeckTreeSnapshotModel::GetChildDeckNodeIndexes(const QModelIndex& Parent) const {
    if (not Parent.isValid()) {
        return m_RootDeckNodeIndexesVector;
    }
    const DeckNode& ParentDeckNode{ TryGetDeckNode(Parent).value().get() };
    return ParentDeckNode.m_ChildDeckNodeIndexesVector;
}

[[nodiscard]] int DeckTreeSnapshotModel::CompareDeckNodes(const std::size_t LeftDeckNodeIndex, const std::size_t RightDeckNodeIndex) const noexcept {
    const DeckNode& LeftDeckNode{ m_DeckNodesVector.at(LeftDeckNodeIndex) };
    const DeckNode& RightDeckNode{ m_DeckNodesVector.at(RightDeckNodeIndex) };
    const auto CompareDeckNodeCounts{ [](const std::uint32_t LeftDeckNodeCount, const std::uint32_t RightDeckNodeCount) static noexcept -> int {
        return static_cast<int>(LeftDeckNodeCount > RightDeckNodeCount) - static_cast<int>(LeftDeckNodeCount < RightDeckNodeCount);
    } };
    switch (m_SortColumn) {
    case static_cast<int>(ColumnEnum::DeckNameColumn):
        return LeftDeckNode.m_DeckTreeSnapshotNode.m_DeckName.compare(RightDeckNode.m_DeckTreeSnapshotNode.m_DeckName);
    case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeSnapshotNode.m_SubtreeDueNowCount, RightDeckNode.m_DeckTreeSnapshotNode.m_SubtreeDueNowCount);
    case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeSnapshotNode.m_SubtreeByTodayCount, RightDeckNode.m_DeckTreeSnapshotNode.m_SubtreeByTodayCount);
    case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeSnapshotNode.m_SubtreeTotalCount, RightDeckNode.m_DeckTreeSnapshotNode.m_SubtreeTotalCount);
    default:
        return 0;
    }
}

void DeckTreeSnapshotModel::SortSiblingDeckNodeIndexes(std::vector<std::size_t>& SiblingDeckNodeIndexes) {
    std::stable_sort(SiblingDeckNodeIndexes.begin(),
                     SiblingDeckNodeIndexes.end(),
                     [this](const std::size_t LeftDeckNodeIndex, const std::size_t RightDeckNodeIndex) noexcept -> bool {
                         const int DeckNodeComparison{ CompareDeckNodes(LeftDeckNodeIndex, RightDeckNodeIndex) };
                         if (m_SortOrder == Qt::AscendingOrder) {
                             return DeckNodeComparison < 0;
                         }
                         return DeckNodeComparison > 0;
                     });
}

void DeckTreeSnapshotModel::UpdateSiblingRowIndexes(const std::optional<std::size_t> ParentDeckNodeIndex) noexcept {
    std::vector<std::size_t>& SiblingDeckNodeIndexes{ ParentDeckNodeIndex.has_value() ?
                                                          m_DeckNodesVector.at(ParentDeckNodeIndex.value()).m_ChildDeckNodeIndexesVector :
                                                          m_RootDeckNodeIndexesVector };
    for (std::size_t SiblingDeckRow{ 0 }; SiblingDeckRow < SiblingDeckNodeIndexes.size(); ++SiblingDeckRow) {
        DeckNode& ChildDeckNode{ m_DeckNodesVector.at(SiblingDeckNodeIndexes.at(SiblingDeckRow)) };
        ChildDeckNode.m_RowInParentIndex = SiblingDeckRow;
        UpdateSiblingRowIndexes(SiblingDeckNodeIndexes.at(SiblingDeckRow));
    }
}

void DeckTreeSnapshotModel::ApplyCurrentSort() {
    if (m_SortColumn < static_cast<int>(ColumnEnum::DeckNameColumn) or m_SortColumn > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
        return;
    }
    SortSiblingDeckNodeIndexes(m_RootDeckNodeIndexesVector);
    for (std::size_t DeckNodeIndex{ 0 }; DeckNodeIndex < m_DeckNodesVector.size(); ++DeckNodeIndex) {
        SortSiblingDeckNodeIndexes(m_DeckNodesVector.at(DeckNodeIndex).m_ChildDeckNodeIndexesVector);
    }
    UpdateSiblingRowIndexes();
}

void DeckTreeSnapshotModel::ReplaceAll(std::vector<Domain::Deck::DeckTreeSnapshotNode> DeckTreeSnapshotNodeVector) noexcept {
    Runtime::TryCatchWrapper([&]() -> void {
        std::vector<DeckNode> DeckNodesVector;
        std::vector<std::size_t> RootDeckNodeIndexesVector;
        std::unordered_map<std::string, std::size_t> DeckNodeIndexByIdHash;
        DeckNodesVector.reserve(DeckTreeSnapshotNodeVector.size());
        RootDeckNodeIndexesVector.reserve(DeckTreeSnapshotNodeVector.size());
        DeckNodeIndexByIdHash.reserve(DeckTreeSnapshotNodeVector.size());
        for (Domain::Deck::DeckTreeSnapshotNode& DeckTreeSnapshotNode : DeckTreeSnapshotNodeVector) {
            const std::size_t DeckNodeIndex{ DeckNodesVector.size() };
            DeckNodeIndexByIdHash.emplace(DeckTreeSnapshotNode.m_DeckId, DeckNodeIndex);
            DeckNodesVector.emplace_back(DeckNode{ std::move(DeckTreeSnapshotNode), std::nullopt, 0, std::vector<std::size_t>{} });
        }
        for (std::size_t DeckNodeIndex{ 0 }; DeckNodeIndex < DeckNodesVector.size(); ++DeckNodeIndex) {
            DeckNode& CurrentDeckNode{ DeckNodesVector.at(DeckNodeIndex) };
            const std::optional<std::string>& ParentId{ CurrentDeckNode.m_DeckTreeSnapshotNode.m_ParentDeckId };
            if (not ParentId.has_value()) {
                CurrentDeckNode.m_RowInParentIndex = RootDeckNodeIndexesVector.size();
                RootDeckNodeIndexesVector.push_back(DeckNodeIndex);
                continue;
            }
            CurrentDeckNode.m_ParentDeckNodeIndex = DeckNodeIndexByIdHash.at(ParentId.value());
            CurrentDeckNode.m_RowInParentIndex = DeckNodesVector.at(CurrentDeckNode.m_ParentDeckNodeIndex.value()).m_ChildDeckNodeIndexesVector.size();
            DeckNodesVector.at(CurrentDeckNode.m_ParentDeckNodeIndex.value()).m_ChildDeckNodeIndexesVector.push_back(DeckNodeIndex);
        }
        beginResetModel();
        m_DeckNodesVector = std::move(DeckNodesVector);
        m_RootDeckNodeIndexesVector = std::move(RootDeckNodeIndexesVector);
        ApplyCurrentSort();
        endResetModel();
    });
}

}
