#include "Presentation/Model/DeckTreeModel.hpp"

#include <algorithm>

namespace Presentation::Model {

[[nodiscard]] const DeckTreeModel::DeckNode* DeckTreeModel::TryGetDeckNode(const QModelIndex& Index) const noexcept {
    if (not Index.isValid()) {
        return nullptr;
    }
    const qsizetype DeckNodeIndex{ static_cast<qsizetype>(Index.internalId()) };
    if (DeckNodeIndex < 0 or DeckNodeIndex >= m_DeckNodesQVector.size()) {
        return nullptr;
    }
    return &m_DeckNodesQVector.at(DeckNodeIndex);
}

[[nodiscard]] const QVector<qsizetype>& DeckTreeModel::GetChildDeckNodeIndexes(const QModelIndex& Parent) const noexcept {
    if (not Parent.isValid()) {
        return m_RootDeckNodeIndexesQVector;
    }
    const DeckNode* ParentDeckNode{ TryGetDeckNode(Parent) };
    if (ParentDeckNode == nullptr) {
        return m_RootDeckNodeIndexesQVector;
    }
    return ParentDeckNode->m_ChildDeckNodeIndexesQVector;
}

[[nodiscard]] int DeckTreeModel::CompareDeckNodes(const qsizetype LeftDeckNodeIndex, const qsizetype RightDeckNodeIndex) const noexcept {
    const DeckNode& LeftDeckNode{ m_DeckNodesQVector.at(LeftDeckNodeIndex) };
    const DeckNode& RightDeckNode{ m_DeckNodesQVector.at(RightDeckNodeIndex) };
    const auto CompareDeckNodeCounts{ [](const quint32 LeftDeckNodeCount, const quint32 RightDeckNodeCount) {
        return static_cast<int>(LeftDeckNodeCount > RightDeckNodeCount) - static_cast<int>(LeftDeckNodeCount < RightDeckNodeCount);
    } };
    switch (m_SortColumn) {
    case static_cast<int>(ColumnEnum::DeckNameColumn):
        return QString::compare(LeftDeckNode.m_DeckNodeData.m_DeckName, RightDeckNode.m_DeckNodeData.m_DeckName, Qt::CaseSensitive);
    case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckNodeData.m_SubtreeDueNowCount, RightDeckNode.m_DeckNodeData.m_SubtreeDueNowCount);
    case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckNodeData.m_SubtreeByTodayCount, RightDeckNode.m_DeckNodeData.m_SubtreeByTodayCount);
    case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckNodeData.m_SubtreeTotalCount, RightDeckNode.m_DeckNodeData.m_SubtreeTotalCount);
    default:
        return 0;
    }
}

void DeckTreeModel::SortSiblingDeckNodeIndexes(QVector<qsizetype>& SiblingDeckNodeIndexes) noexcept {
    std::stable_sort(
        SiblingDeckNodeIndexes.begin(), SiblingDeckNodeIndexes.end(), [this](const qsizetype LeftDeckNodeIndex, const qsizetype RightDeckNodeIndex) {
            const int DeckNodeComparison{ CompareDeckNodes(LeftDeckNodeIndex, RightDeckNodeIndex) };
            if (m_SortOrder == Qt::AscendingOrder) {
                return DeckNodeComparison < 0;
            }
            return DeckNodeComparison > 0;
        });
}

void DeckTreeModel::UpdateSiblingRowIndexes(const qsizetype ParentDeckNodeIndex) noexcept {
    QVector<qsizetype>& SiblingDeckNodeIndexes{ ParentDeckNodeIndex == s_RootDeckNodeIndex ?
                                                    m_RootDeckNodeIndexesQVector :
                                                    m_DeckNodesQVector[ParentDeckNodeIndex].m_ChildDeckNodeIndexesQVector };
    for (qsizetype SiblingDeckRow{ 0 }; SiblingDeckRow < SiblingDeckNodeIndexes.size(); ++SiblingDeckRow) {
        DeckNode& ChildDeckNode{ m_DeckNodesQVector[SiblingDeckNodeIndexes.at(SiblingDeckRow)] };
        ChildDeckNode.m_RowInParentIndex = SiblingDeckRow;
        UpdateSiblingRowIndexes(SiblingDeckNodeIndexes.at(SiblingDeckRow));
    }
}

void DeckTreeModel::ApplyCurrentSort() noexcept {
    if (m_SortColumn < static_cast<int>(ColumnEnum::DeckNameColumn) or m_SortColumn > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
        return;
    }
    SortSiblingDeckNodeIndexes(m_RootDeckNodeIndexesQVector);
    for (qsizetype DeckNodeIndex{ 0 }; DeckNodeIndex < m_DeckNodesQVector.size(); ++DeckNodeIndex) {
        SortSiblingDeckNodeIndexes(m_DeckNodesQVector[DeckNodeIndex].m_ChildDeckNodeIndexesQVector);
    }
    UpdateSiblingRowIndexes();
}

void DeckTreeModel::ReplaceAll(QVector<DeckNodeData> DeckNodeDataQVector) {
    QVector<DeckNode> DeckNodesQVector;
    QVector<qsizetype> RootDeckNodeIndexesQVector;
    QHash<QString, qsizetype> DeckNodeIndexByIdQHash;
    DeckNodesQVector.reserve(DeckNodeDataQVector.size());
    RootDeckNodeIndexesQVector.reserve(DeckNodeDataQVector.size());
    DeckNodeIndexByIdQHash.reserve(DeckNodeDataQVector.size());
    for (DeckNodeData& DeckNodeData : DeckNodeDataQVector) {
        const qsizetype DeckNodeIndex{ DeckNodesQVector.size() };
        const QString DeckId{ DeckNodeData.m_DeckId };
        DeckNodesQVector.emplace_back(DeckNode{ std::move(DeckNodeData), s_RootDeckNodeIndex, -1, QVector<qsizetype>{} });
        DeckNodeIndexByIdQHash.insert(DeckId, DeckNodeIndex);
    }
    for (qsizetype DeckNodeIndex{ 0 }; DeckNodeIndex < DeckNodesQVector.size(); ++DeckNodeIndex) {
        DeckNode& CurrentDeckNode{ DeckNodesQVector[DeckNodeIndex] };
        const std::optional<QString>& ParentId{ CurrentDeckNode.m_DeckNodeData.m_ParentDeckId };
        if (not ParentId.has_value()) {
            CurrentDeckNode.m_RowInParentIndex = RootDeckNodeIndexesQVector.size();
            RootDeckNodeIndexesQVector.push_back(DeckNodeIndex);
            continue;
        }
        CurrentDeckNode.m_ParentDeckNodeIndex = DeckNodeIndexByIdQHash.constFind(ParentId.value()).value();
        CurrentDeckNode.m_RowInParentIndex = DeckNodesQVector.at(CurrentDeckNode.m_ParentDeckNodeIndex).m_ChildDeckNodeIndexesQVector.size();
        DeckNodesQVector[CurrentDeckNode.m_ParentDeckNodeIndex].m_ChildDeckNodeIndexesQVector.push_back(DeckNodeIndex);
    }
    beginResetModel();
    m_DeckNodesQVector = std::move(DeckNodesQVector);
    m_RootDeckNodeIndexesQVector = std::move(RootDeckNodeIndexesQVector);
    ApplyCurrentSort();
    endResetModel();
}

}
