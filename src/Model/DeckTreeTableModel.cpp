#include "Model/DeckTreeTableModel.hpp"

#include <algorithm>

namespace Model {

const DeckTreeTableModel::DeckNode* DeckTreeTableModel::TryGetDeckNode(const QModelIndex& Index) const noexcept {
    if (not Index.isValid()) {
        return nullptr;
    }
    const qsizetype DeckNodeIndex{ static_cast<qsizetype>(Index.internalId()) };
    if (DeckNodeIndex < 0 or DeckNodeIndex >= m_DeckNodes.size()) {
        return nullptr;
    }
    return &m_DeckNodes.at(DeckNodeIndex);
}

const DeckTreeTableModel::DeckNode* DeckTreeTableModel::TryGetDeckNode(const QString& DeckId) const noexcept {
    const auto DeckNodeIterator{ m_DeckNodeIndexById.constFind(DeckId) };
    if (DeckNodeIterator == m_DeckNodeIndexById.cend()) {
        return nullptr;
    }
    return &m_DeckNodes.at(DeckNodeIterator.value());
}

const QVector<qsizetype>& DeckTreeTableModel::GetChildDeckNodeIndexes(const QModelIndex& Parent) const noexcept {
    if (not Parent.isValid()) {
        return m_RootDeckNodeIndexes;
    }
    const DeckNode* ParentDeckNode{ TryGetDeckNode(Parent) };
    if (ParentDeckNode == nullptr) {
        return m_RootDeckNodeIndexes;
    }
    return ParentDeckNode->m_ChildDeckNodeIndexes;
}

bool DeckTreeTableModel::HasDuplicateSiblingName(const QString& DeckName, const QString& ParentDeckId, const QString& IgnoredDeckId) const noexcept {
    for (const DeckNode& DeckNode : m_DeckNodes) {
        if (DeckNode.m_Data.m_ParentId == ParentDeckId and DeckNode.m_Data.m_Name == DeckName and DeckNode.m_Data.m_Id != IgnoredDeckId) {
            return true;
        }
    }
    return false;
}

bool DeckTreeTableModel::WouldCreateDeckHaveDuplicateSiblingName(const QString& DeckName, const QString& ParentDeckId) const noexcept {
    return HasDuplicateSiblingName(DeckName, ParentDeckId);
}

bool DeckTreeTableModel::WouldMoveDeckCreateCycle(const QString& DeckId, const QString& ParentDeckId) const noexcept {
    if (ParentDeckId.isEmpty()) {
        return false;
    }
    if (DeckId == ParentDeckId) {
        return true;
    }

    const DeckNode* CurrentDeckNode{ TryGetDeckNode(ParentDeckId) };
    while (CurrentDeckNode not_eq nullptr) {
        if (CurrentDeckNode->m_Data.m_Id == DeckId) {
            return true;
        }
        if (CurrentDeckNode->m_ParentDeckNodeIndex == s_RootDeckNodeIndex) {
            break;
        }
        CurrentDeckNode = &m_DeckNodes.at(CurrentDeckNode->m_ParentDeckNodeIndex);
    }
    return false;
}

bool DeckTreeTableModel::WouldMoveDeckHaveDuplicateSiblingName(const QString& DeckId, const QString& ParentDeckId) const noexcept {
    const DeckNode* CurrentDeckNode{ TryGetDeckNode(DeckId) };
    if (CurrentDeckNode == nullptr) {
        return false;
    }
    return HasDuplicateSiblingName(CurrentDeckNode->m_Data.m_Name, ParentDeckId, DeckId);
}

bool DeckTreeTableModel::WouldUpdateDeckNameHaveDuplicateSiblingName(const QString& DeckId, const QString& DeckName) const noexcept {
    const DeckNode* CurrentDeckNode{ TryGetDeckNode(DeckId) };
    if (CurrentDeckNode == nullptr) {
        return false;
    }
    return HasDuplicateSiblingName(DeckName, CurrentDeckNode->m_Data.m_ParentId, DeckId);
}

int DeckTreeTableModel::CompareDeckNodes(const qsizetype LeftDeckNodeIndex, const qsizetype RightDeckNodeIndex) const noexcept {
    const DeckNode& LeftDeckNode{ m_DeckNodes.at(LeftDeckNodeIndex) };
    const DeckNode& RightDeckNode{ m_DeckNodes.at(RightDeckNodeIndex) };

    switch (m_SortColumn) {
    case NameColumn: {
        const int NameComparison{ QString::compare(LeftDeckNode.m_Data.m_Name, RightDeckNode.m_Data.m_Name, Qt::CaseInsensitive) };
        if (NameComparison not_eq 0) {
            return NameComparison;
        }
        break;
    }
    case DueNowColumn:
        if (LeftDeckNode.m_Data.m_DueNow < RightDeckNode.m_Data.m_DueNow) {
            return -1;
        }
        if (LeftDeckNode.m_Data.m_DueNow > RightDeckNode.m_Data.m_DueNow) {
            return 1;
        }
        break;
    case ByTodayColumn:
        if (LeftDeckNode.m_Data.m_ByToday < RightDeckNode.m_Data.m_ByToday) {
            return -1;
        }
        if (LeftDeckNode.m_Data.m_ByToday > RightDeckNode.m_Data.m_ByToday) {
            return 1;
        }
        break;
    case TotalColumn:
        if (LeftDeckNode.m_Data.m_Total < RightDeckNode.m_Data.m_Total) {
            return -1;
        }
        if (LeftDeckNode.m_Data.m_Total > RightDeckNode.m_Data.m_Total) {
            return 1;
        }
        break;
    default:
        return 0;
    }

    return QString::compare(LeftDeckNode.m_Data.m_Name, RightDeckNode.m_Data.m_Name, Qt::CaseInsensitive);
}

void DeckTreeTableModel::SortSiblingDeckNodeIndexes(QVector<qsizetype>& SiblingDeckNodeIndexes) noexcept {
    std::sort(SiblingDeckNodeIndexes.begin(), SiblingDeckNodeIndexes.end(), [this](const qsizetype LeftDeckNodeIndex, const qsizetype RightDeckNodeIndex) {
        const int DeckNodeComparison{ CompareDeckNodes(LeftDeckNodeIndex, RightDeckNodeIndex) };
        if (DeckNodeComparison == 0) {
            return LeftDeckNodeIndex < RightDeckNodeIndex;
        }
        if (m_SortOrder == Qt::AscendingOrder) {
            return DeckNodeComparison < 0;
        }
        return DeckNodeComparison > 0;
    });
}

void DeckTreeTableModel::UpdateSiblingRowIndexes(const qsizetype ParentDeckNodeIndex) noexcept {
    QVector<qsizetype>& SiblingDeckNodeIndexes{ ParentDeckNodeIndex == s_RootDeckNodeIndex ? m_RootDeckNodeIndexes :
                                                                                             m_DeckNodes[ParentDeckNodeIndex].m_ChildDeckNodeIndexes };

    for (qsizetype SiblingDeckRow = 0; SiblingDeckRow < SiblingDeckNodeIndexes.size(); ++SiblingDeckRow) {
        DeckNode& ChildDeckNode{ m_DeckNodes[SiblingDeckNodeIndexes.at(SiblingDeckRow)] };
        ChildDeckNode.m_RowInParent = SiblingDeckRow;
        UpdateSiblingRowIndexes(SiblingDeckNodeIndexes.at(SiblingDeckRow));
    }
}

void DeckTreeTableModel::ApplyCurrentSort() noexcept {
    if (m_SortColumn < NameColumn or m_SortColumn > TotalColumn) {
        return;
    }

    SortSiblingDeckNodeIndexes(m_RootDeckNodeIndexes);
    for (qsizetype DeckNodeIndex = 0; DeckNodeIndex < m_DeckNodes.size(); ++DeckNodeIndex) {
        SortSiblingDeckNodeIndexes(m_DeckNodes[DeckNodeIndex].m_ChildDeckNodeIndexes);
    }
    UpdateSiblingRowIndexes();
}

void DeckTreeTableModel::ReplaceAll(QVector<DeckNodeData> DeckNodes) {
    beginResetModel();

    m_DeckNodes.clear();
    m_RootDeckNodeIndexes.clear();
    m_DeckNodeIndexById.clear();

    m_DeckNodes.reserve(DeckNodes.size());
    m_RootDeckNodeIndexes.reserve(DeckNodes.size());
    m_DeckNodeIndexById.reserve(DeckNodes.size());

    for (DeckNodeData& DeckNodeData : DeckNodes) {
        const qsizetype DeckNodeIndex{ m_DeckNodes.size() };
        m_DeckNodes.emplace_back(DeckNode{ std::move(DeckNodeData), s_RootDeckNodeIndex, s_RootDeckNodeIndex, {} });
        m_DeckNodeIndexById.insert(m_DeckNodes.back().m_Data.m_Id, DeckNodeIndex);
    }

    for (qsizetype DeckNodeIndex = 0; DeckNodeIndex < m_DeckNodes.size(); ++DeckNodeIndex) {
        DeckNode& CurrentDeckNode{ m_DeckNodes[DeckNodeIndex] };
        const QString& ParentId{ CurrentDeckNode.m_Data.m_ParentId };
        if (ParentId.isEmpty()) {
            CurrentDeckNode.m_RowInParent = m_RootDeckNodeIndexes.size();
            m_RootDeckNodeIndexes.push_back(DeckNodeIndex);
            continue;
        }

        const auto ParentDeckNodeIterator{ m_DeckNodeIndexById.constFind(ParentId) };
        if (ParentDeckNodeIterator == m_DeckNodeIndexById.cend() or ParentDeckNodeIterator.value() == DeckNodeIndex) {
            CurrentDeckNode.m_ParentDeckNodeIndex = s_RootDeckNodeIndex;
            CurrentDeckNode.m_RowInParent = m_RootDeckNodeIndexes.size();
            m_RootDeckNodeIndexes.push_back(DeckNodeIndex);
            continue;
        }

        CurrentDeckNode.m_ParentDeckNodeIndex = ParentDeckNodeIterator.value();
        CurrentDeckNode.m_RowInParent = m_DeckNodes.at(CurrentDeckNode.m_ParentDeckNodeIndex).m_ChildDeckNodeIndexes.size();
        m_DeckNodes[CurrentDeckNode.m_ParentDeckNodeIndex].m_ChildDeckNodeIndexes.push_back(DeckNodeIndex);
    }

    ApplyCurrentSort();

    endResetModel();
}

}
