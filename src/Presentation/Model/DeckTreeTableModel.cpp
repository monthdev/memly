#include "Presentation/Model/DeckTreeTableModel.hpp"

#include <algorithm>

#include "Support/Fatal.hpp"

namespace Presentation::Model {

const DeckTreeTableModel::DeckNode* DeckTreeTableModel::TryGetDeckNode(const QModelIndex& Index) const noexcept {
    if (not Index.isValid()) {
        return nullptr;
    }
    const qsizetype DeckNodeIndex{ static_cast<qsizetype>(Index.internalId()) };
    if (DeckNodeIndex < 0 or DeckNodeIndex >= m_DeckNodesQVector.size()) {
        return nullptr;
    }
    return &m_DeckNodesQVector.at(DeckNodeIndex);
}

const DeckTreeTableModel::DeckNode* DeckTreeTableModel::TryGetDeckNode(const QString& DeckId) const noexcept {
    const auto DeckNodeIdIndexIterator{ m_DeckNodeIndexByIdQHash.constFind(DeckId) };
    if (DeckNodeIdIndexIterator == m_DeckNodeIndexByIdQHash.cend()) {
        return nullptr;
    }
    return &m_DeckNodesQVector.at(DeckNodeIdIndexIterator.value());
}

const QVector<qsizetype>& DeckTreeTableModel::GetChildDeckNodeIndexes(const QModelIndex& Parent) const noexcept {
    if (not Parent.isValid()) {
        return m_RootDeckNodeIndexesQVector;
    }
    const DeckNode* ParentDeckNode{ TryGetDeckNode(Parent) };
    if (ParentDeckNode == nullptr) {
        return m_RootDeckNodeIndexesQVector;
    }
    return ParentDeckNode->m_ChildDeckNodeIndexesQVector;
}

std::optional<std::reference_wrapper<const DeckTreeTableModel::DeckNodeData>> DeckTreeTableModel::TryGetDeckNodeData(const QString& DeckId) const noexcept {
    const DeckNode* DeckNode{ TryGetDeckNode(DeckId) };
    if (DeckNode == nullptr) {
        return std::nullopt;
    }
    return DeckNode->m_Data;
}

bool DeckTreeTableModel::HasDuplicateSiblingName(const QString& DeckName, const QString& ParentDeckId, const QString& DeckId) const noexcept {
    const QVector<qsizetype>* SiblingDeckNodeIndexes{ &m_RootDeckNodeIndexesQVector };
    if (not ParentDeckId.isEmpty()) {
        const DeckNode* ParentDeckNode{ TryGetDeckNode(ParentDeckId) };
        if (ParentDeckNode == nullptr) {
            return false;
        }
        SiblingDeckNodeIndexes = &ParentDeckNode->m_ChildDeckNodeIndexesQVector;
    }
    for (const qsizetype SiblingDeckNodeIndex : *SiblingDeckNodeIndexes) {
        const DeckNode& SiblingDeckNode{ m_DeckNodesQVector.at(SiblingDeckNodeIndex) };
        if (SiblingDeckNode.m_Data.m_Name == DeckName and SiblingDeckNode.m_Data.m_Id != DeckId) {
            return true;
        }
    }
    return false;
}

bool DeckTreeTableModel::WouldReparentCreateCycle(const QString& DeckId, const QString& NewParentDeckId) const noexcept {
    const DeckNode* CurrentDeckNode{ TryGetDeckNode(NewParentDeckId) };
    while (CurrentDeckNode not_eq nullptr) {
        if (CurrentDeckNode->m_Data.m_Id == DeckId) {
            return true;
        }
        if (CurrentDeckNode->m_ParentDeckNodeIndex == s_RootDeckNodeIndex) {
            break;
        }
        CurrentDeckNode = &m_DeckNodesQVector.at(CurrentDeckNode->m_ParentDeckNodeIndex);
    }
    return false;
}

bool DeckTreeTableModel::WouldReparentCreateTargetLanguageMismatch(const QString& DeckId, const QString& NewParentDeckId) const noexcept {
    if (NewParentDeckId.isEmpty()) {
        return false;
    }
    const DeckNode* CurrentDeckNode{ TryGetDeckNode(DeckId) };
    const DeckNode* NewParentDeckNode{ TryGetDeckNode(NewParentDeckId) };
    if (CurrentDeckNode == nullptr or NewParentDeckNode == nullptr) {
        return false;
    }
    return CurrentDeckNode->m_Data.m_TargetLanguageCode not_eq NewParentDeckNode->m_Data.m_TargetLanguageCode;
}

int DeckTreeTableModel::CompareDeckNodes(const qsizetype LeftDeckNodeIndex, const qsizetype RightDeckNodeIndex) const noexcept {
    const DeckNode& LeftDeckNode{ m_DeckNodesQVector.at(LeftDeckNodeIndex) };
    const DeckNode& RightDeckNode{ m_DeckNodesQVector.at(RightDeckNodeIndex) };
    const auto CompareDeckNodeCounts{ [](const quint32 LeftDeckNodeCount, const quint32 RightDeckNodeCount) {
        return static_cast<int>(LeftDeckNodeCount > RightDeckNodeCount) - static_cast<int>(LeftDeckNodeCount < RightDeckNodeCount);
    } };
    switch (m_SortColumn) {
    case NameColumn:
        return QString::compare(LeftDeckNode.m_Data.m_Name, RightDeckNode.m_Data.m_Name, Qt::CaseSensitive);
    case DueNowCountColumn:
        return CompareDeckNodeCounts(LeftDeckNode.m_Data.m_DueNowCount, RightDeckNode.m_Data.m_DueNowCount);
    case ByTodayCountColumn:
        return CompareDeckNodeCounts(LeftDeckNode.m_Data.m_ByTodayCount, RightDeckNode.m_Data.m_ByTodayCount);
    case TotalCountColumn:
        return CompareDeckNodeCounts(LeftDeckNode.m_Data.m_TotalCount, RightDeckNode.m_Data.m_TotalCount);
    default:
        return 0;
    }
}

void DeckTreeTableModel::SortSiblingDeckNodeIndexes(QVector<qsizetype>& SiblingDeckNodeIndexes) noexcept {
    std::stable_sort(
        SiblingDeckNodeIndexes.begin(), SiblingDeckNodeIndexes.end(), [this](const qsizetype LeftDeckNodeIndex, const qsizetype RightDeckNodeIndex) {
            const int DeckNodeComparison{ CompareDeckNodes(LeftDeckNodeIndex, RightDeckNodeIndex) };
            if (m_SortOrder == Qt::AscendingOrder) {
                return DeckNodeComparison < 0;
            }
            return DeckNodeComparison > 0;
        });
}

void DeckTreeTableModel::UpdateSiblingRowIndexes(const qsizetype ParentDeckNodeIndex) noexcept {
    QVector<qsizetype>& SiblingDeckNodeIndexes{ ParentDeckNodeIndex == s_RootDeckNodeIndex ?
                                                    m_RootDeckNodeIndexesQVector :
                                                    m_DeckNodesQVector[ParentDeckNodeIndex].m_ChildDeckNodeIndexesQVector };
    for (qsizetype SiblingDeckRow{ 0 }; SiblingDeckRow < SiblingDeckNodeIndexes.size(); ++SiblingDeckRow) {
        DeckNode& ChildDeckNode{ m_DeckNodesQVector[SiblingDeckNodeIndexes.at(SiblingDeckRow)] };
        ChildDeckNode.m_RowInParentIndex = SiblingDeckRow;
        UpdateSiblingRowIndexes(SiblingDeckNodeIndexes.at(SiblingDeckRow));
    }
}

void DeckTreeTableModel::ApplyCurrentSort() noexcept {
    if (m_SortColumn < NameColumn or m_SortColumn > TotalCountColumn) {
        return;
    }
    SortSiblingDeckNodeIndexes(m_RootDeckNodeIndexesQVector);
    for (qsizetype DeckNodeIndex{ 0 }; DeckNodeIndex < m_DeckNodesQVector.size(); ++DeckNodeIndex) {
        SortSiblingDeckNodeIndexes(m_DeckNodesQVector[DeckNodeIndex].m_ChildDeckNodeIndexesQVector);
    }
    UpdateSiblingRowIndexes();
}

void DeckTreeTableModel::ValidateNoDeckNodeCycles(const QVector<DeckNode>& DeckNodesQVector) const {
    constexpr quint8 Unvisited{ 0 };
    constexpr quint8 Visiting{ 1 };
    constexpr quint8 Visited{ 2 };
    QVector<quint8> DeckNodeVisitStateQVector(DeckNodesQVector.size(), Unvisited);
    QVector<qsizetype> CurrentDeckNodePathQVector;
    CurrentDeckNodePathQVector.reserve(DeckNodesQVector.size());
    for (qsizetype DeckNodeIndex{ 0 }; DeckNodeIndex < DeckNodesQVector.size(); ++DeckNodeIndex) {
        CurrentDeckNodePathQVector.clear();
        qsizetype CurrentDeckNodeIndex{ DeckNodeIndex };
        while (CurrentDeckNodeIndex not_eq s_RootDeckNodeIndex) {
            const quint8 CurrentDeckNodeVisitState{ DeckNodeVisitStateQVector.at(CurrentDeckNodeIndex) };
            if (CurrentDeckNodeVisitState == Visited) {
                break;
            }
            if (CurrentDeckNodeVisitState == Visiting) {
                Support::ThrowError("Cycle detected in deck hierarchy snapshot");
            }
            DeckNodeVisitStateQVector[CurrentDeckNodeIndex] = Visiting;
            CurrentDeckNodePathQVector.push_back(CurrentDeckNodeIndex);
            CurrentDeckNodeIndex = DeckNodesQVector.at(CurrentDeckNodeIndex).m_ParentDeckNodeIndex;
        }
        for (const qsizetype CurrentDeckNodePathIndex : CurrentDeckNodePathQVector) { DeckNodeVisitStateQVector[CurrentDeckNodePathIndex] = Visited; }
    }
}

void DeckTreeTableModel::ValidateUniqueSiblingDeckNodeNames(const QVector<DeckNode>& DeckNodesQVector,
                                                            const QVector<qsizetype>& SiblingDeckNodeIndexesQVector) const {
    QHash<QString, qsizetype> DeckNodeIndexByNameQHash;
    DeckNodeIndexByNameQHash.reserve(SiblingDeckNodeIndexesQVector.size());
    for (const qsizetype SiblingDeckNodeIndex : SiblingDeckNodeIndexesQVector) {
        const QString& DeckName{ DeckNodesQVector.at(SiblingDeckNodeIndex).m_Data.m_Name };
        if (DeckNodeIndexByNameQHash.contains(DeckName)) {
            Support::ThrowError("Duplicate sibling deck name in deck hierarchy snapshot");
        }
        DeckNodeIndexByNameQHash.insert(DeckName, SiblingDeckNodeIndex);
    }
}

void DeckTreeTableModel::ReplaceAll(QVector<DeckNodeData> DeckNodeDataQVector) {
    QVector<DeckNode> DeckNodesQVector;
    QVector<qsizetype> RootDeckNodeIndexesQVector;
    QHash<QString, qsizetype> DeckNodeIndexByIdQHash;
    DeckNodesQVector.reserve(DeckNodeDataQVector.size());
    RootDeckNodeIndexesQVector.reserve(DeckNodeDataQVector.size());
    DeckNodeIndexByIdQHash.reserve(DeckNodeDataQVector.size());
    for (DeckNodeData& DeckNodeData : DeckNodeDataQVector) {
        const qsizetype DeckNodeIndex{ DeckNodesQVector.size() };
        DeckNodesQVector.emplace_back(DeckNode{ std::move(DeckNodeData), s_RootDeckNodeIndex, -1, {} });
        if (DeckNodeIndexByIdQHash.contains(DeckNodeData.m_Id)) {
            Support::ThrowError("Duplicate deck id in deck hierarchy snapshot");
        }
        DeckNodeIndexByIdQHash.insert(DeckNodesQVector.back().m_Data.m_Id, DeckNodeIndex);
    }
    for (qsizetype DeckNodeIndex{ 0 }; DeckNodeIndex < DeckNodesQVector.size(); ++DeckNodeIndex) {
        DeckNode& CurrentDeckNode{ DeckNodesQVector[DeckNodeIndex] };
        const QString& ParentId{ CurrentDeckNode.m_Data.m_ParentId };
        if (ParentId.isEmpty()) {
            CurrentDeckNode.m_RowInParentIndex = RootDeckNodeIndexesQVector.size();
            RootDeckNodeIndexesQVector.push_back(DeckNodeIndex);
            continue;
        }
        const auto ParentDeckNodeIdIndexIterator{ DeckNodeIndexByIdQHash.constFind(ParentId) };
        if (ParentDeckNodeIdIndexIterator == DeckNodeIndexByIdQHash.cend() or ParentDeckNodeIdIndexIterator.value() == DeckNodeIndex) {
            Support::ThrowError("Invalid parent deck id in deck hierarchy snapshot");
        }
        CurrentDeckNode.m_ParentDeckNodeIndex = ParentDeckNodeIdIndexIterator.value();
        CurrentDeckNode.m_RowInParentIndex = DeckNodesQVector.at(CurrentDeckNode.m_ParentDeckNodeIndex).m_ChildDeckNodeIndexesQVector.size();
        DeckNodesQVector[CurrentDeckNode.m_ParentDeckNodeIndex].m_ChildDeckNodeIndexesQVector.push_back(DeckNodeIndex);
    }
    ValidateNoDeckNodeCycles(DeckNodesQVector);
    ValidateUniqueSiblingDeckNodeNames(DeckNodesQVector, RootDeckNodeIndexesQVector);
    for (const DeckNode& DeckNode : DeckNodesQVector) { ValidateUniqueSiblingDeckNodeNames(DeckNodesQVector, DeckNode.m_ChildDeckNodeIndexesQVector); }
    beginResetModel();
    m_DeckNodesQVector = std::move(DeckNodesQVector);
    m_RootDeckNodeIndexesQVector = std::move(RootDeckNodeIndexesQVector);
    m_DeckNodeIndexByIdQHash = std::move(DeckNodeIndexByIdQHash);
    ApplyCurrentSort();
    endResetModel();
}

}
