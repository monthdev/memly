// Temporarily disabled during deck forest model refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Presentation/Model/DeckForestModel.hpp"

#include <qstring.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "Support/Runtime/Exception/ExceptionBoundary.hpp"
#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Presentation::Model {

DeckForestModel::DeckNode::DeckNode(Application::Domain::Deck::Index::DeckForestSnapshotNode&& DeckForestSnapshotNode,
                                    std::optional<std::size_t>&& ParentDeckNodeIndexOptional,
                                    const std::size_t RowInParentIndex,
                                    std::vector<std::size_t>&& ChildDeckNodeIndexesVector) noexcept
    : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
    , m_DeckForestSnapshotNode{ std::move(DeckForestSnapshotNode) }
    , m_ParentDeckNodeIndexOptional{ std::move(ParentDeckNodeIndexOptional) }
    , m_RowInParentIndex{ RowInParentIndex }
    , m_ChildDeckNodeIndexesVector{ std::move(ChildDeckNodeIndexesVector) } {
}

DeckForestModel::DeckForestModel(QObject* Parent) noexcept
    : QAbstractItemModel{ Parent }
    , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_DeckNodesVector{}
    , m_RootDeckNodeIndexesVector{}
    , m_SortColumn{ s_UnsortedColumn }
    , m_SortOrder{ Qt::AscendingOrder } {
}

[[nodiscard]] QModelIndex DeckForestModel::index(const int Row, const int Column, const QModelIndex& Parent) const noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> QModelIndex {
        if (not this->hasIndex(Row, Column, Parent)) {
            return QModelIndex{};
        }
        const std::vector<std::size_t>& ChildDeckNodeIndexesVector{ this->GetChildDeckNodeIndexes(Parent) };
        const std::size_t ChildDeckNodeIndex{ ChildDeckNodeIndexesVector.at(static_cast<std::size_t>(Row)) };
        return this->createIndex(Row, Column, static_cast<quintptr>(ChildDeckNodeIndex));
    });
}

[[nodiscard]] QModelIndex DeckForestModel::parent(const QModelIndex& Index) const noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> QModelIndex {
        const std::optional<std::reference_wrapper<const DeckNode>> CurrentDeckNodeOptional{ this->TryGetDeckNode(Index) };
        if (not CurrentDeckNodeOptional.has_value()) {
            return QModelIndex{};
        }
        const DeckNode& CurrentDeckNode{ CurrentDeckNodeOptional.value().get() };
        if (not CurrentDeckNode.m_ParentDeckNodeIndexOptional.has_value()) {
            return QModelIndex{};
        }
        const std::size_t ParentDeckNodeIndex{ CurrentDeckNode.m_ParentDeckNodeIndexOptional.value() };
        const DeckNode& ParentDeckNode{ this->m_DeckNodesVector.at(ParentDeckNodeIndex) };
        return this->createIndex(static_cast<int>(ParentDeckNode.m_RowInParentIndex), 0, static_cast<quintptr>(ParentDeckNodeIndex));
    });
}

[[nodiscard]] int DeckForestModel::rowCount(const QModelIndex& Parent) const noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> int {
        if (Parent.column() > 0) {
            return 0;
        }
        return static_cast<int>(this->GetChildDeckNodeIndexes(Parent).size());
    });
}

[[nodiscard]] int DeckForestModel::columnCount(const QModelIndex& Parent) const noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> int {
        if (Parent.isValid() and Parent.column() > 0) {
            return 0;
        }
        return static_cast<int>(ColumnEnum::SubtreeTotalCountColumn) + 1;
    });
}

[[nodiscard]] QVariant DeckForestModel::data(const QModelIndex& Index, const int Role) const noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> QVariant {
        const std::optional<std::reference_wrapper<const DeckNode>> CurrentDeckNodeOptional{ this->TryGetDeckNode(Index) };
        if (not CurrentDeckNodeOptional.has_value()) {
            return QVariant{};
        }
        const DeckNode& CurrentDeckNode{ CurrentDeckNodeOptional.value().get() };
        switch (Role) {
        case Qt::DisplayRole:
            switch (Index.column()) {
            case static_cast<int>(ColumnEnum::DeckNameColumn):
                return QString::fromStdString(CurrentDeckNode.m_DeckForestSnapshotNode.m_DeckName);
            case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
                return CurrentDeckNode.m_DeckForestSnapshotNode.m_SubtreeDueNowCount;
            case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
                return CurrentDeckNode.m_DeckForestSnapshotNode.m_SubtreeByTodayCount;
            case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
                return CurrentDeckNode.m_DeckForestSnapshotNode.m_SubtreeTotalCount;
            default:
                return QVariant{};
            }
        case static_cast<int>(RoleEnum::DeckIdRole):
            return QString::fromStdString(CurrentDeckNode.m_DeckForestSnapshotNode.m_DeckId);
        case static_cast<int>(RoleEnum::ParentDeckIdRole):
            if (not CurrentDeckNode.m_DeckForestSnapshotNode.m_ParentDeckIdOptional.has_value()) {
                return QString{};
            }
            return QString::fromStdString(CurrentDeckNode.m_DeckForestSnapshotNode.m_ParentDeckIdOptional.value());
        case static_cast<int>(RoleEnum::DeckNameRole):
            return QString::fromStdString(CurrentDeckNode.m_DeckForestSnapshotNode.m_DeckName);
        case static_cast<int>(RoleEnum::CreatedAtMillisecondsSinceEpochRole):
            return CurrentDeckNode.m_DeckForestSnapshotNode.m_CreatedAtMillisecondsSinceEpoch;
        case static_cast<int>(RoleEnum::LastUpdatedAtMillisecondsSinceEpochRole):
            if (not CurrentDeckNode.m_DeckForestSnapshotNode.m_LastUpdatedAtMillisecondsSinceEpochOptional.has_value()) {
                return std::int64_t{};
            }
            return CurrentDeckNode.m_DeckForestSnapshotNode.m_LastUpdatedAtMillisecondsSinceEpochOptional.value();
        case static_cast<int>(RoleEnum::SelfDueNowCountRole):
            return CurrentDeckNode.m_DeckForestSnapshotNode.m_SelfDueNowCount;
        case static_cast<int>(RoleEnum::SelfByTodayCountRole):
            return CurrentDeckNode.m_DeckForestSnapshotNode.m_SelfByTodayCount;
        case static_cast<int>(RoleEnum::SelfTotalCountRole):
            return CurrentDeckNode.m_DeckForestSnapshotNode.m_SelfTotalCount;
        case static_cast<int>(RoleEnum::SubtreeDueNowCountRole):
            return CurrentDeckNode.m_DeckForestSnapshotNode.m_SubtreeDueNowCount;
        case static_cast<int>(RoleEnum::SubtreeByTodayCountRole):
            return CurrentDeckNode.m_DeckForestSnapshotNode.m_SubtreeByTodayCount;
        case static_cast<int>(RoleEnum::SubtreeTotalCountRole):
            return CurrentDeckNode.m_DeckForestSnapshotNode.m_SubtreeTotalCount;
        case static_cast<int>(RoleEnum::TargetLanguageCodeRole):
            return static_cast<unsigned int>(CurrentDeckNode.m_DeckForestSnapshotNode.m_TargetLanguageCode);
        default:
            return QVariant{};
        }
    });
}

[[nodiscard]] bool DeckForestModel::hasChildren(const QModelIndex& Parent) const noexcept {
    return Support::Runtime::Exception::TryCatchWrapper([&]() -> bool {
        if (Parent.column() > 0) {
            return false;
        }
        return this->rowCount(Parent) > 0;
    });
}

void DeckForestModel::sort(const int Column, const Qt::SortOrder SortOrder) noexcept {
    Support::Runtime::Exception::TryCatchWrapper([&]() -> void {
        if (Column < static_cast<int>(ColumnEnum::DeckNameColumn) or Column > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
            return;
        }
        this->m_SortColumn = Column;
        this->m_SortOrder = SortOrder;
        this->beginResetModel();
        this->ApplyCurrentSort();
        this->endResetModel();
    });
}

[[nodiscard]] std::optional<std::reference_wrapper<const DeckForestModel::DeckNode>> DeckForestModel::TryGetDeckNode(const QModelIndex& Index) const noexcept {
    if (not Index.isValid()) {
        return std::nullopt;
    }
    const std::size_t DeckNodeIndex{ static_cast<std::size_t>(Index.internalId()) };
    assert(DeckNodeIndex < this->m_DeckNodesVector.size());
    return this->m_DeckNodesVector.at(DeckNodeIndex);
}

[[nodiscard]] const std::vector<std::size_t>& DeckForestModel::GetChildDeckNodeIndexes(const QModelIndex& Parent) const {
    if (not Parent.isValid()) {
        return this->m_RootDeckNodeIndexesVector;
    }
    const DeckNode& ParentDeckNode{ this->TryGetDeckNode(Parent).value().get() };
    return ParentDeckNode.m_ChildDeckNodeIndexesVector;
}

[[nodiscard]] int DeckForestModel::CompareDeckNodes(const std::size_t LeftDeckNodeIndex, const std::size_t RightDeckNodeIndex) const noexcept {
    const DeckNode& LeftDeckNode{ this->m_DeckNodesVector.at(LeftDeckNodeIndex) };
    const DeckNode& RightDeckNode{ this->m_DeckNodesVector.at(RightDeckNodeIndex) };
    const auto CompareDeckNodeCounts{ [](const std::uint32_t LeftDeckNodeCount, const std::uint32_t RightDeckNodeCount) static noexcept -> int {
        return static_cast<int>(LeftDeckNodeCount > RightDeckNodeCount) - static_cast<int>(LeftDeckNodeCount < RightDeckNodeCount);
    } };
    switch (this->m_SortColumn) {
    case static_cast<int>(ColumnEnum::DeckNameColumn):
        return LeftDeckNode.m_DeckForestSnapshotNode.m_DeckName.compare(RightDeckNode.m_DeckForestSnapshotNode.m_DeckName);
    case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckForestSnapshotNode.m_SubtreeDueNowCount, RightDeckNode.m_DeckForestSnapshotNode.m_SubtreeDueNowCount);
    case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckForestSnapshotNode.m_SubtreeByTodayCount, RightDeckNode.m_DeckForestSnapshotNode.m_SubtreeByTodayCount);
    case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
        return CompareDeckNodeCounts(LeftDeckNode.m_DeckForestSnapshotNode.m_SubtreeTotalCount, RightDeckNode.m_DeckForestSnapshotNode.m_SubtreeTotalCount);
    default:
        return 0;
    }
}

void DeckForestModel::SortSiblingDeckNodeIndexes(std::vector<std::size_t>& SiblingDeckNodeIndexes) {
    std::stable_sort(SiblingDeckNodeIndexes.begin(),
                     SiblingDeckNodeIndexes.end(),
                     [this](const std::size_t LeftDeckNodeIndex, const std::size_t RightDeckNodeIndex) noexcept -> bool {
                         const int DeckNodeComparison{ this->CompareDeckNodes(LeftDeckNodeIndex, RightDeckNodeIndex) };
                         if (this->m_SortOrder == Qt::AscendingOrder) {
                             return DeckNodeComparison < 0;
                         }
                         return DeckNodeComparison > 0;
                     });
}

void DeckForestModel::UpdateSiblingRowIndexes(const std::optional<std::size_t>& ParentDeckNodeIndexOptional) noexcept {
    std::vector<std::size_t>& SiblingDeckNodeIndexes{ ParentDeckNodeIndexOptional.has_value() ?
                                                          this->m_DeckNodesVector.at(ParentDeckNodeIndexOptional.value()).m_ChildDeckNodeIndexesVector :
                                                          this->m_RootDeckNodeIndexesVector };
    for (std::size_t SiblingDeckRow{ 0 }; SiblingDeckRow < SiblingDeckNodeIndexes.size(); ++SiblingDeckRow) {
        DeckNode& ChildDeckNode{ this->m_DeckNodesVector.at(SiblingDeckNodeIndexes.at(SiblingDeckRow)) };
        ChildDeckNode.m_RowInParentIndex = SiblingDeckRow;
        this->UpdateSiblingRowIndexes(SiblingDeckNodeIndexes.at(SiblingDeckRow));
    }
}

void DeckForestModel::ApplyCurrentSort() {
    if (this->m_SortColumn < static_cast<int>(ColumnEnum::DeckNameColumn) or this->m_SortColumn > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
        return;
    }
    this->SortSiblingDeckNodeIndexes(this->m_RootDeckNodeIndexesVector);
    for (std::size_t DeckNodeIndex{ 0 }; DeckNodeIndex < this->m_DeckNodesVector.size(); ++DeckNodeIndex) {
        this->SortSiblingDeckNodeIndexes(this->m_DeckNodesVector.at(DeckNodeIndex).m_ChildDeckNodeIndexesVector);
    }
    this->UpdateSiblingRowIndexes();
}

void DeckForestModel::ReplaceAll(std::vector<Application::Domain::Deck::Index::DeckForestSnapshotNode>&& DeckForestSnapshotNodeVector) noexcept {
    Support::Runtime::Exception::TryCatchWrapper([&]() -> void {
        std::vector<DeckNode> DeckNodesVector;
        std::vector<std::size_t> RootDeckNodeIndexesVector;
        std::unordered_map<std::string, std::size_t> DeckNodeIndexByIdHash;
        DeckNodesVector.reserve(DeckForestSnapshotNodeVector.size());
        RootDeckNodeIndexesVector.reserve(DeckForestSnapshotNodeVector.size());
        DeckNodeIndexByIdHash.reserve(DeckForestSnapshotNodeVector.size());
        for (Application::Domain::Deck::Index::DeckForestSnapshotNode& DeckForestSnapshotNode : DeckForestSnapshotNodeVector) {
            const std::size_t DeckNodeIndex{ DeckNodesVector.size() };
            DeckNodeIndexByIdHash.emplace(DeckForestSnapshotNode.m_DeckId, DeckNodeIndex);
            DeckNodesVector.emplace_back(DeckNode{ std::move(DeckForestSnapshotNode), std::nullopt, 0, std::vector<std::size_t>{} });
        }
        for (std::size_t DeckNodeIndex{ 0 }; DeckNodeIndex < DeckNodesVector.size(); ++DeckNodeIndex) {
            DeckNode& CurrentDeckNode{ DeckNodesVector.at(DeckNodeIndex) };
            const std::optional<std::string>& ParentIdOptional{ CurrentDeckNode.m_DeckForestSnapshotNode.m_ParentDeckIdOptional };
            if (not ParentIdOptional.has_value()) {
                CurrentDeckNode.m_RowInParentIndex = RootDeckNodeIndexesVector.size();
                RootDeckNodeIndexesVector.push_back(DeckNodeIndex);
                continue;
            }
            CurrentDeckNode.m_ParentDeckNodeIndexOptional = DeckNodeIndexByIdHash.at(ParentIdOptional.value());
            CurrentDeckNode.m_RowInParentIndex = DeckNodesVector.at(CurrentDeckNode.m_ParentDeckNodeIndexOptional.value()).m_ChildDeckNodeIndexesVector.size();
            DeckNodesVector.at(CurrentDeckNode.m_ParentDeckNodeIndexOptional.value()).m_ChildDeckNodeIndexesVector.push_back(DeckNodeIndex);
        }
        this->beginResetModel();
        this->m_DeckNodesVector = std::move(DeckNodesVector);
        this->m_RootDeckNodeIndexesVector = std::move(RootDeckNodeIndexesVector);
        this->ApplyCurrentSort();
        this->endResetModel();
    });
}

}
#endif
