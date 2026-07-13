#if 0
// Temporarily disabled during deck tree model refactor.
#include "Presentation/Model/DeckTreeModel.hpp"

#include <QString>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "Support/Runtime/ExceptionBoundary.hpp"

namespace Presentation::Model {

[[nodiscard]] QModelIndex DeckTreeModel::index(const int Row, const int Column, const QModelIndex& Parent) const noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> QModelIndex {
        if (not hasIndex(Row, Column, Parent)) {
            return QModelIndex{};
        }
        const std::vector<std::size_t>& ChildDeckNodeIndexesVector{ GetChildDeckNodeIndexes(Parent) };
        const std::size_t ChildDeckNodeIndex{ ChildDeckNodeIndexesVector.at(static_cast<std::size_t>(Row)) };
        return createIndex(Row, Column, static_cast<quintptr>(ChildDeckNodeIndex));
    });
}

[[nodiscard]] QModelIndex DeckTreeModel::parent(const QModelIndex& Index) const noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> QModelIndex {
        const std::optional<std::reference_wrapper<const DeckNode>> CurrentDeckNodeOptional{ TryGetDeckNode(Index) };
        if (not CurrentDeckNodeOptional.has_value()) {
            return QModelIndex{};
        }
        const DeckNode& CurrentDeckNode{ CurrentDeckNodeOptional.value().get() };
        if (not CurrentDeckNode.m_ParentDeckNodeIndex.has_value()) {
            return QModelIndex{};
        }
        const std::size_t ParentDeckNodeIndex{ CurrentDeckNode.m_ParentDeckNodeIndex.value() };
        const DeckNode& ParentDeckNode{ m_DeckNodesVector.at(ParentDeckNodeIndex) };
        return createIndex(static_cast<int>(ParentDeckNode.m_RowInParentIndex), 0, static_cast<quintptr>(ParentDeckNodeIndex));
    });
}

[[nodiscard]] int DeckTreeModel::rowCount(const QModelIndex& Parent) const noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> int {
        if (Parent.column() > 0) {
            return 0;
        }
        return static_cast<int>(GetChildDeckNodeIndexes(Parent).size());
    });
}

[[nodiscard]] int DeckTreeModel::columnCount(const QModelIndex& Parent) const noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> int {
        if (Parent.isValid() and Parent.column() > 0) {
            return 0;
        }
        return static_cast<int>(ColumnEnum::SubtreeTotalCountColumn) + 1;
    });
}

[[nodiscard]] QVariant DeckTreeModel::data(const QModelIndex& Index, const int Role) const noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> QVariant {
        const std::optional<std::reference_wrapper<const DeckNode>> CurrentDeckNodeOptional{ TryGetDeckNode(Index) };
        if (not CurrentDeckNodeOptional.has_value()) {
            return QVariant{};
        }
        const DeckNode& CurrentDeckNode{ CurrentDeckNodeOptional.value().get() };
        switch (Role) {
        case Qt::DisplayRole:
            switch (Index.column()) {
            case static_cast<int>(ColumnEnum::DeckNameColumn):
                return QString::fromStdString(CurrentDeckNode.m_DeckTreeNode.m_DeckName);
            case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
                return CurrentDeckNode.m_DeckTreeNode.m_SubtreeDueNowCount;
            case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
                return CurrentDeckNode.m_DeckTreeNode.m_SubtreeByTodayCount;
            case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
                return CurrentDeckNode.m_DeckTreeNode.m_SubtreeTotalCount;
            default:
                return QVariant{};
            }
        case static_cast<int>(RoleEnum::DeckIdRole):
            return QString::fromStdString(CurrentDeckNode.m_DeckTreeNode.m_DeckId);
        case static_cast<int>(RoleEnum::ParentDeckIdRole):
            if (not CurrentDeckNode.m_DeckTreeNode.m_ParentDeckIdOptional.has_value()) {
                return QString{};
            }
            return QString::fromStdString(CurrentDeckNode.m_DeckTreeNode.m_ParentDeckIdOptional.value());
        case static_cast<int>(RoleEnum::DeckNameRole):
            return QString::fromStdString(CurrentDeckNode.m_DeckTreeNode.m_DeckName);
        case static_cast<int>(RoleEnum::CreatedAtMillisecondsSinceEpochRole):
            return CurrentDeckNode.m_DeckTreeNode.m_CreatedAtMillisecondsSinceEpoch;
        case static_cast<int>(RoleEnum::LastUpdatedAtMillisecondsSinceEpochRole):
            if (not CurrentDeckNode.m_DeckTreeNode.m_LastUpdatedAtMillisecondsSinceEpochOptional.has_value()) {
                return std::int64_t{};
            }
            return CurrentDeckNode.m_DeckTreeNode.m_LastUpdatedAtMillisecondsSinceEpochOptional.value();
        case static_cast<int>(RoleEnum::SelfDueNowCountRole):
            return CurrentDeckNode.m_DeckTreeNode.m_SelfDueNowCount;
        case static_cast<int>(RoleEnum::SelfByTodayCountRole):
            return CurrentDeckNode.m_DeckTreeNode.m_SelfByTodayCount;
        case static_cast<int>(RoleEnum::SelfTotalCountRole):
            return CurrentDeckNode.m_DeckTreeNode.m_SelfTotalCount;
        case static_cast<int>(RoleEnum::SubtreeDueNowCountRole):
            return CurrentDeckNode.m_DeckTreeNode.m_SubtreeDueNowCount;
        case static_cast<int>(RoleEnum::SubtreeByTodayCountRole):
            return CurrentDeckNode.m_DeckTreeNode.m_SubtreeByTodayCount;
        case static_cast<int>(RoleEnum::SubtreeTotalCountRole):
            return CurrentDeckNode.m_DeckTreeNode.m_SubtreeTotalCount;
        case static_cast<int>(RoleEnum::TargetLanguageCodeRole):
            return static_cast<unsigned int>(CurrentDeckNode.m_DeckTreeNode.m_TargetLanguageCode);
        default:
            return QVariant{};
        }
    });
}

[[nodiscard]] bool DeckTreeModel::hasChildren(const QModelIndex& Parent) const noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> bool {
        if (Parent.column() > 0) {
            return false;
        }
        return rowCount(Parent) > 0;
    });
}

void DeckTreeModel::sort(const int Column, const Qt::SortOrder SortOrder) noexcept {
    Support::Runtime::TryCatchWrapper([&]() -> void {
        if (Column < static_cast<int>(ColumnEnum::DeckNameColumn) or Column > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
            return;
        }
        m_SortColumn = Column;
        m_SortOrder = SortOrder;
        beginResetModel();
        ApplyCurrentSort();
        endResetModel();
    });
}

[[nodiscard]] std::optional<std::reference_wrapper<const DeckTreeModel::DeckNode>> DeckTreeModel::TryGetDeckNode(const QModelIndex& Index) const noexcept {
    if (not Index.isValid()) {
        return std::nullopt;
    }
    const std::size_t DeckNodeIndex{ static_cast<std::size_t>(Index.internalId()) };
    assert(DeckNodeIndex < m_DeckNodesVector.size());
    return m_DeckNodesVector.at(DeckNodeIndex);
}

[[nodiscard]] const std::vector<std::size_t>& DeckTreeModel::GetChildDeckNodeIndexes(const QModelIndex& Parent) const {
    if (not Parent.isValid()) {
        return m_RootDeckNodeIndexesVector;
    }
    const DeckNode& ParentDeckNode{ TryGetDeckNode(Parent).value().get() };
    return ParentDeckNode.m_ChildDeckNodeIndexesVector;
}

[[nodiscard]] int DeckTreeModel::CompareDeckNodes(const std::size_t LeftDeckNodeIndex, const std::size_t RightDeckNodeIndex) const noexcept {
    const DeckNode& LeftDeckNode{ m_DeckNodesVector.at(LeftDeckNodeIndex) };
    const DeckNode& RightDeckNode{ m_DeckNodesVector.at(RightDeckNodeIndex) };
    const auto CompareDeckNodeCounts{ [](const std::uint32_t LeftDeckNodeCount, const std::uint32_t RightDeckNodeCount) static noexcept -> int {
        return static_cast<int>(LeftDeckNodeCount > RightDeckNodeCount) - static_cast<int>(LeftDeckNodeCount < RightDeckNodeCount);
    } };
    switch (m_SortColumn) {
    case static_cast<int>(ColumnEnum::DeckNameColumn):
        return LeftDeckNode.m_DeckTreeNode.m_DeckName.compare(RightDeckNode.m_DeckTreeNode.m_DeckName);
    case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeNode.m_SubtreeDueNowCount, RightDeckNode.m_DeckTreeNode.m_SubtreeDueNowCount);
    case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeNode.m_SubtreeByTodayCount, RightDeckNode.m_DeckTreeNode.m_SubtreeByTodayCount);
    case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckTreeNode.m_SubtreeTotalCount, RightDeckNode.m_DeckTreeNode.m_SubtreeTotalCount);
    default:
        return 0;
    }
}

void DeckTreeModel::SortSiblingDeckNodeIndexes(std::vector<std::size_t>& SiblingDeckNodeIndexes) {
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

void DeckTreeModel::UpdateSiblingRowIndexes(const std::optional<std::size_t>& ParentDeckNodeIndex) noexcept {
    std::vector<std::size_t>& SiblingDeckNodeIndexes{ ParentDeckNodeIndex.has_value() ?
                                                          m_DeckNodesVector.at(ParentDeckNodeIndex.value()).m_ChildDeckNodeIndexesVector :
                                                          m_RootDeckNodeIndexesVector };
    for (std::size_t SiblingDeckRow{ 0 }; SiblingDeckRow < SiblingDeckNodeIndexes.size(); ++SiblingDeckRow) {
        DeckNode& ChildDeckNode{ m_DeckNodesVector.at(SiblingDeckNodeIndexes.at(SiblingDeckRow)) };
        ChildDeckNode.m_RowInParentIndex = SiblingDeckRow;
        UpdateSiblingRowIndexes(SiblingDeckNodeIndexes.at(SiblingDeckRow));
    }
}

void DeckTreeModel::ApplyCurrentSort() {
    if (m_SortColumn < static_cast<int>(ColumnEnum::DeckNameColumn) or m_SortColumn > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
        return;
    }
    SortSiblingDeckNodeIndexes(m_RootDeckNodeIndexesVector);
    for (std::size_t DeckNodeIndex{ 0 }; DeckNodeIndex < m_DeckNodesVector.size(); ++DeckNodeIndex) {
        SortSiblingDeckNodeIndexes(m_DeckNodesVector.at(DeckNodeIndex).m_ChildDeckNodeIndexesVector);
    }
    UpdateSiblingRowIndexes();
}

void DeckTreeModel::ReplaceAll(std::vector<Application::Domain::Deck::Data::DeckTreeSnapshotNode>&& DeckTreeNodeVector) noexcept {
    Support::Runtime::TryCatchWrapper([&]() -> void {
        std::vector<DeckNode> DeckNodesVector;
        std::vector<std::size_t> RootDeckNodeIndexesVector;
        std::unordered_map<std::string, std::size_t> DeckNodeIndexByIdHash;
        DeckNodesVector.reserve(DeckTreeNodeVector.size());
        RootDeckNodeIndexesVector.reserve(DeckTreeNodeVector.size());
        DeckNodeIndexByIdHash.reserve(DeckTreeNodeVector.size());
        for (Application::Domain::Deck::Data::DeckTreeSnapshotNode& DeckTreeNode : DeckTreeNodeVector) {
            const std::size_t DeckNodeIndex{ DeckNodesVector.size() };
            DeckNodeIndexByIdHash.emplace(DeckTreeNode.m_DeckId, DeckNodeIndex);
            DeckNodesVector.emplace_back(DeckNode{ std::move(DeckTreeNode), std::nullopt, 0, std::vector<std::size_t>{} });
        }
        for (std::size_t DeckNodeIndex{ 0 }; DeckNodeIndex < DeckNodesVector.size(); ++DeckNodeIndex) {
            DeckNode& CurrentDeckNode{ DeckNodesVector.at(DeckNodeIndex) };
            const std::optional<std::string>& ParentIdOptional{ CurrentDeckNode.m_DeckTreeNode.m_ParentDeckIdOptional };
            if (not ParentIdOptional.has_value()) {
                CurrentDeckNode.m_RowInParentIndex = RootDeckNodeIndexesVector.size();
                RootDeckNodeIndexesVector.push_back(DeckNodeIndex);
                continue;
            }
            CurrentDeckNode.m_ParentDeckNodeIndex = DeckNodeIndexByIdHash.at(ParentIdOptional.value());
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
#endif
