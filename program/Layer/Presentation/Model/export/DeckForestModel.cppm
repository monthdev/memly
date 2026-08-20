export module Memly.Model:DeckForestModel;

// Temporarily disabled during deck forest model refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <qabstractitemmodel.h>
#include <qvariant.h>

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

import Memly.Domain;

export namespace Memly::Model {

class DeckForestModel final : public QAbstractItemModel {
    Q_OBJECT

private:
    struct DeckNode {
        Domain::DeckForestSnapshotNode m_DeckForestSnapshotNode;
        std::optional<std::size_t> m_ParentDeckNodeIndexOptional;
        std::size_t m_RowInParentIndex;
        std::vector<std::size_t> m_ChildDeckNodeIndexesVector;

        explicit DeckNode(Domain::DeckForestSnapshotNode&& DeckForestSnapshotNode,
                          std::optional<std::size_t>&& ParentDeckNodeIndexOptional,
                          const std::size_t RowInParentIndex,
                          std::vector<std::size_t>&& ChildDeckNodeIndexesVector) noexcept
            : m_DeckForestSnapshotNode{ std::move(DeckForestSnapshotNode) }
            , m_ParentDeckNodeIndexOptional{ std::move(ParentDeckNodeIndexOptional) }
            , m_RowInParentIndex{ RowInParentIndex }
            , m_ChildDeckNodeIndexesVector{ std::move(ChildDeckNodeIndexesVector) } {
        }

        explicit DeckNode(const DeckNode&) = delete;
        auto operator=(const DeckNode&) -> DeckNode& = delete;

        explicit DeckNode(DeckNode&&) noexcept = default;
        auto operator=(DeckNode&&) -> DeckNode& = delete;

        ~DeckNode() noexcept = default;
    };

    static constexpr int s_UnsortedColumn{ -1 };
    std::vector<DeckNode> m_DeckNodesVector;
    std::vector<std::size_t> m_RootDeckNodeIndexesVector;
    int m_SortColumn;
    Qt::SortOrder m_SortOrder;

public:
    explicit DeckForestModel(QObject* = nullptr) noexcept;

    explicit DeckForestModel(const DeckForestModel&) = delete;
    auto operator=(const DeckForestModel&) -> DeckForestModel& = delete;

    explicit DeckForestModel(DeckForestModel&&) = delete;
    auto operator=(DeckForestModel&&) -> DeckForestModel& = delete;

    ~DeckForestModel() noexcept override = default;

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

    void ReplaceAll(std::vector<Domain::DeckForestSnapshotNode>&&) noexcept;

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
