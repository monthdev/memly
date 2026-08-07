// Temporarily disabled during deck forest model refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qabstractitemmodel.h>
#include <qvariant.h>

#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Presentation::Model {

class DeckForestModel final : public QAbstractItemModel, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    Q_OBJECT

private:
    struct DeckNode : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
        Application::Domain::Deck::Index::DeckForestSnapshotNode m_DeckForestSnapshotNode;
        std::optional<std::size_t> m_ParentDeckNodeIndexOptional;
        std::size_t m_RowInParentIndex;
        std::vector<std::size_t> m_ChildDeckNodeIndexesVector;

        explicit DeckNode(Application::Domain::Deck::Index::DeckForestSnapshotNode&&,
                          std::optional<std::size_t>&&,
                          std::size_t,
                          std::vector<std::size_t>&&) noexcept;
    };

    static constexpr int s_UnsortedColumn{ -1 };
    std::vector<DeckNode> m_DeckNodesVector;
    std::vector<std::size_t> m_RootDeckNodeIndexesVector;
    int m_SortColumn;
    Qt::SortOrder m_SortOrder;

public:
    explicit DeckForestModel(QObject* = nullptr) noexcept;
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
