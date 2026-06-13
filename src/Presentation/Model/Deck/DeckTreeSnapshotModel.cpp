#include "Presentation/Model/Deck/DeckTreeSnapshotModel.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

#include "Runtime/Crash.hpp"

namespace Presentation::Model::Deck {

[[nodiscard]] std::optional<std::reference_wrapper<const DeckTreeSnapshotModel::DeckNode>>
DeckTreeSnapshotModel::TryGetDeckNode(const QModelIndex& Index) const noexcept {
    if (not Index.isValid()) {
        return std::nullopt;
    }
    const qsizetype DeckNodeIndex{ static_cast<qsizetype>(Index.internalId()) };
    Q_ASSERT(DeckNodeIndex >= 0 and DeckNodeIndex < static_cast<qsizetype>(m_DeckNodesVector.size()));
    return m_DeckNodesVector.at(static_cast<std::size_t>(DeckNodeIndex));
}

[[nodiscard]] const QVector<qsizetype>& DeckTreeSnapshotModel::GetChildDeckNodeIndexes(const QModelIndex& Parent) const {
    if (not Parent.isValid()) {
        return m_RootDeckNodeIndexesQVector;
    }
    const DeckNode& ParentDeckNode{ TryGetDeckNode(Parent).value().get() };
    return ParentDeckNode.m_ChildDeckNodeIndexesQVector;
}

[[nodiscard]] int DeckTreeSnapshotModel::CompareDeckNodes(const qsizetype LeftDeckNodeIndex, const qsizetype RightDeckNodeIndex) const noexcept {
    const DeckNode& LeftDeckNode{ m_DeckNodesVector.at(static_cast<std::size_t>(LeftDeckNodeIndex)) };
    const DeckNode& RightDeckNode{ m_DeckNodesVector.at(static_cast<std::size_t>(RightDeckNodeIndex)) };
    const auto CompareDeckNodeCounts{ [](const quint32 LeftDeckNodeCount, const quint32 RightDeckNodeCount) static noexcept -> int {
        return static_cast<int>(LeftDeckNodeCount > RightDeckNodeCount) - static_cast<int>(LeftDeckNodeCount < RightDeckNodeCount);
    } };
    switch (m_SortColumn) {
    case static_cast<int>(ColumnEnum::DeckNameColumn):
        return QString::compare(LeftDeckNode.m_DeckTreeSnapshotNodeData.m_DeckName, RightDeckNode.m_DeckTreeSnapshotNodeData.m_DeckName, Qt::CaseSensitive);
    case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeSnapshotNodeData.m_SubtreeDueNowCount,
                                     RightDeckNode.m_DeckTreeSnapshotNodeData.m_SubtreeDueNowCount);
    case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeSnapshotNodeData.m_SubtreeByTodayCount,
                                     RightDeckNode.m_DeckTreeSnapshotNodeData.m_SubtreeByTodayCount);
    case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeSnapshotNodeData.m_SubtreeTotalCount, RightDeckNode.m_DeckTreeSnapshotNodeData.m_SubtreeTotalCount);
    default:
        return 0;
    }
}

void DeckTreeSnapshotModel::SortSiblingDeckNodeIndexes(QVector<qsizetype>& SiblingDeckNodeIndexes) {
    std::stable_sort(SiblingDeckNodeIndexes.begin(),
                     SiblingDeckNodeIndexes.end(),
                     [this](const qsizetype LeftDeckNodeIndex, const qsizetype RightDeckNodeIndex) noexcept -> bool {
                         const int DeckNodeComparison{ CompareDeckNodes(LeftDeckNodeIndex, RightDeckNodeIndex) };
                         if (m_SortOrder == Qt::AscendingOrder) {
                             return DeckNodeComparison < 0;
                         }
                         return DeckNodeComparison > 0;
                     });
}

void DeckTreeSnapshotModel::UpdateSiblingRowIndexes(const qsizetype ParentDeckNodeIndex) noexcept {
    QVector<qsizetype>& SiblingDeckNodeIndexes{ ParentDeckNodeIndex == s_RootDeckNodeIndex ?
                                                    m_RootDeckNodeIndexesQVector :
                                                    m_DeckNodesVector[static_cast<std::size_t>(ParentDeckNodeIndex)].m_ChildDeckNodeIndexesQVector };
    for (qsizetype SiblingDeckRow{ 0 }; SiblingDeckRow < SiblingDeckNodeIndexes.size(); ++SiblingDeckRow) {
        DeckNode& ChildDeckNode{ m_DeckNodesVector[static_cast<std::size_t>(SiblingDeckNodeIndexes.at(SiblingDeckRow))] };
        ChildDeckNode.m_RowInParentIndex = SiblingDeckRow;
        UpdateSiblingRowIndexes(SiblingDeckNodeIndexes.at(SiblingDeckRow));
    }
}

void DeckTreeSnapshotModel::ApplyCurrentSort() {
    if (m_SortColumn < static_cast<int>(ColumnEnum::DeckNameColumn) or m_SortColumn > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
        return;
    }
    SortSiblingDeckNodeIndexes(m_RootDeckNodeIndexesQVector);
    for (qsizetype DeckNodeIndex{ 0 }; DeckNodeIndex < static_cast<qsizetype>(m_DeckNodesVector.size()); ++DeckNodeIndex) {
        SortSiblingDeckNodeIndexes(m_DeckNodesVector[static_cast<std::size_t>(DeckNodeIndex)].m_ChildDeckNodeIndexesQVector);
    }
    UpdateSiblingRowIndexes();
}

void DeckTreeSnapshotModel::ReplaceAll(std::vector<Domain::Deck::DeckTreeSnapshotNodeData> DeckTreeSnapshotNodeDataVector) noexcept {
    Runtime::TryCatchWrapper([&]() -> void {
        std::vector<DeckNode> DeckNodesVector;
        QVector<qsizetype> RootDeckNodeIndexesQVector;
        QHash<QString, qsizetype> DeckNodeIndexByIdQHash;
        DeckNodesVector.reserve(DeckTreeSnapshotNodeDataVector.size());
        RootDeckNodeIndexesQVector.reserve(static_cast<qsizetype>(DeckTreeSnapshotNodeDataVector.size()));
        DeckNodeIndexByIdQHash.reserve(static_cast<qsizetype>(DeckTreeSnapshotNodeDataVector.size()));
        for (Domain::Deck::DeckTreeSnapshotNodeData& DeckTreeSnapshotNodeData : DeckTreeSnapshotNodeDataVector) {
            const qsizetype DeckNodeIndex{ static_cast<qsizetype>(DeckNodesVector.size()) };
            const QString DeckId{ DeckTreeSnapshotNodeData.m_DeckId };
            DeckNodesVector.emplace_back(DeckNode{ std::move(DeckTreeSnapshotNodeData), s_RootDeckNodeIndex, -1, QVector<qsizetype>{} });
            DeckNodeIndexByIdQHash.insert(DeckId, DeckNodeIndex);
        }
        for (qsizetype DeckNodeIndex{ 0 }; DeckNodeIndex < static_cast<qsizetype>(DeckNodesVector.size()); ++DeckNodeIndex) {
            DeckNode& CurrentDeckNode{ DeckNodesVector[static_cast<std::size_t>(DeckNodeIndex)] };
            const std::optional<QString>& ParentId{ CurrentDeckNode.m_DeckTreeSnapshotNodeData.m_ParentDeckId };
            if (not ParentId.has_value()) {
                CurrentDeckNode.m_RowInParentIndex = RootDeckNodeIndexesQVector.size();
                RootDeckNodeIndexesQVector.push_back(DeckNodeIndex);
                continue;
            }
            CurrentDeckNode.m_ParentDeckNodeIndex = DeckNodeIndexByIdQHash.constFind(ParentId.value()).value();
            CurrentDeckNode.m_RowInParentIndex =
                DeckNodesVector.at(static_cast<std::size_t>(CurrentDeckNode.m_ParentDeckNodeIndex)).m_ChildDeckNodeIndexesQVector.size();
            DeckNodesVector[static_cast<std::size_t>(CurrentDeckNode.m_ParentDeckNodeIndex)].m_ChildDeckNodeIndexesQVector.push_back(DeckNodeIndex);
        }
        beginResetModel();
        m_DeckNodesVector = std::move(DeckNodesVector);
        m_RootDeckNodeIndexesQVector = std::move(RootDeckNodeIndexesQVector);
        ApplyCurrentSort();
        endResetModel();
    });
}

}
