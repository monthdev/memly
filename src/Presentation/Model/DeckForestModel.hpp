// Temporarily disabled during deck forest model refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qabstractitemmodel.h>
#include <qvariant.h>

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

#include "Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Presentation::Model {

class DeckForestModel final : public QAbstractItemModel, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    Q_OBJECT

private:
    struct DeckNode : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
        Application::Domain::Deck::Index::DeckForestSnapshotNode m_DeckForestSnapshotNode;
        std::optional<std::size_t> m_ParentDeckNodeIndexOptional;
        std::size_t m_RowInParentIndex;
        std::vector<std::size_t> m_ChildDeckNodeIndexesVector;

        explicit DeckNode(Application::Domain::Deck::Index::DeckForestSnapshotNode&& DeckForestSnapshotNode,
                 std::optional<std::size_t>&& ParentDeckNodeIndexOptional,
                 const std::size_t RowInParentIndex,
                 std::vector<std::size_t>&& ChildDeckNodeIndexesVector) noexcept
            : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
            , m_DeckForestSnapshotNode{ std::move(DeckForestSnapshotNode) }
            , m_ParentDeckNodeIndexOptional{ std::move(ParentDeckNodeIndexOptional) }
            , m_RowInParentIndex{ RowInParentIndex }
            , m_ChildDeckNodeIndexesVector{ std::move(ChildDeckNodeIndexesVector) } {
        }
    };

    std::vector<DeckNode> m_DeckNodesVector;
    std::vector<std::size_t> m_RootDeckNodeIndexesVector;
    int m_SortColumn;
    Qt::SortOrder m_SortOrder;

public:
    explicit DeckForestModel(QObject* Parent = nullptr) noexcept
        : QAbstractItemModel{ Parent }
        , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DeckNodesVector{}
        , m_RootDeckNodeIndexesVector{}
        , m_SortColumn{ -1 }
        , m_SortOrder{ Qt::AscendingOrder } {
    }
    enum class RoleEnum : int {
        DeckIdRole = Qt::UserRole + 1,
        ParentDeckIdRole,
        DeckNameRole,
        CreatedAtMillisecondsSinceEpochRole,
        LastUpdatedAtMillisecondsSinceEpochRole,
        SelfDueNowCountRole,
        SelfByTodayCountRole,
        SelfTotalCountRole,
        SubtreeDueNowCountRole,
        SubtreeByTodayCountRole,
        SubtreeTotalCountRole,
        TargetLanguageCodeRole
    };

    enum class ColumnEnum : int {
        DeckNameColumn = 0,
        SubtreeDueNowCountColumn,
        SubtreeByTodayCountColumn,
        SubtreeTotalCountColumn
    };

    [[nodiscard]] QModelIndex index(int, int, const QModelIndex& = QModelIndex{}) const noexcept override;
    [[nodiscard]] QModelIndex parent(const QModelIndex&) const noexcept override;
    [[nodiscard]] int rowCount(const QModelIndex& = QModelIndex{}) const noexcept override;
    [[nodiscard]] int columnCount(const QModelIndex& = QModelIndex{}) const noexcept override;
    [[nodiscard]] QVariant data(const QModelIndex&, int) const noexcept override;
    [[nodiscard]] bool hasChildren(const QModelIndex& = QModelIndex{}) const noexcept override;
    void sort(int, Qt::SortOrder = Qt::AscendingOrder) noexcept override;

    void ReplaceAll(std::vector<Application::Domain::Deck::Index::DeckForestSnapshotNode>&&) noexcept;

private:
    [[nodiscard]] std::optional<std::reference_wrapper<const DeckNode>> TryGetDeckNode(const QModelIndex&) const noexcept;
    [[nodiscard]] const std::vector<std::size_t>& GetChildDeckNodeIndexes(const QModelIndex&) const;
    void ApplyCurrentSort();
    void SortSiblingDeckNodeIndexes(std::vector<std::size_t>&);
    void UpdateSiblingRowIndexes(const std::optional<std::size_t>& = std::nullopt) noexcept;
    [[nodiscard]] int CompareDeckNodes(std::size_t, std::size_t) const noexcept;
};
}
#endif
