#if 0
// Temporarily disabled during deck tree model refactor.
#pragma once

#include <QAbstractItemModel>
#include <QVariant>
#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

#include "Application/Domain/Deck/Data/DeckTreeSnapshotNode.hpp"

namespace Presentation::Model {

class DeckTreeModel final : public QAbstractItemModel {
    Q_OBJECT

public:
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

    explicit DeckTreeModel(QObject* Parent = nullptr) noexcept
        : QAbstractItemModel{ Parent }
        , m_SortColumn{ -1 }
        , m_SortOrder{ Qt::AscendingOrder } {
    }
    DeckTreeModel(const DeckTreeModel&) = delete;
    DeckTreeModel(DeckTreeModel&&) = delete;
    DeckTreeModel& operator=(const DeckTreeModel&) = delete;
    DeckTreeModel& operator=(DeckTreeModel&&) = delete;

    [[nodiscard]] QModelIndex index(int, int, const QModelIndex& = QModelIndex{}) const noexcept override;
    [[nodiscard]] QModelIndex parent(const QModelIndex&) const noexcept override;
    [[nodiscard]] int rowCount(const QModelIndex& = QModelIndex{}) const noexcept override;
    [[nodiscard]] int columnCount(const QModelIndex& = QModelIndex{}) const noexcept override;
    [[nodiscard]] QVariant data(const QModelIndex&, int) const noexcept override;
    [[nodiscard]] bool hasChildren(const QModelIndex& = QModelIndex{}) const noexcept override;
    void sort(int, Qt::SortOrder = Qt::AscendingOrder) noexcept override;

    void ReplaceAll(std::vector<Application::Domain::Deck::Data::DeckTreeSnapshotNode>&&) noexcept;

private:
    struct DeckNode {
        Application::Domain::Deck::Data::DeckTreeSnapshotNode m_DeckTreeNode;
        std::optional<std::size_t> m_ParentDeckNodeIndex;
        std::size_t m_RowInParentIndex;
        std::vector<std::size_t> m_ChildDeckNodeIndexesVector;
    };

    std::vector<DeckNode> m_DeckNodesVector;
    std::vector<std::size_t> m_RootDeckNodeIndexesVector;
    int m_SortColumn;
    Qt::SortOrder m_SortOrder;

    [[nodiscard]] std::optional<std::reference_wrapper<const DeckNode>> TryGetDeckNode(const QModelIndex&) const noexcept;
    [[nodiscard]] const std::vector<std::size_t>& GetChildDeckNodeIndexes(const QModelIndex&) const;
    void ApplyCurrentSort();
    void SortSiblingDeckNodeIndexes(std::vector<std::size_t>&);
    void UpdateSiblingRowIndexes(const std::optional<std::size_t>& = std::nullopt) noexcept;
    [[nodiscard]] int CompareDeckNodes(std::size_t, std::size_t) const noexcept;
};
}
#endif
