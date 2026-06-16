#pragma once

#include <QAbstractItemModel>
#include <QByteArray>
#include <QHash>
#include <QString>
#include <QVariant>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include "Domain/Deck/DeckTreeSnapshotNode.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Model::Deck {

class DeckTreeSnapshotModel final : public QAbstractItemModel {
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

    explicit DeckTreeSnapshotModel(QObject* Parent = nullptr) noexcept
        : QAbstractItemModel{ Parent }
        , m_SortColumn{ -1 }
        , m_SortOrder{ Qt::AscendingOrder } {
    }

    ~DeckTreeSnapshotModel() noexcept override = default;
    DeckTreeSnapshotModel(const DeckTreeSnapshotModel&) = delete;
    DeckTreeSnapshotModel(DeckTreeSnapshotModel&&) = delete;
    DeckTreeSnapshotModel& operator=(const DeckTreeSnapshotModel&) = delete;
    DeckTreeSnapshotModel& operator=(DeckTreeSnapshotModel&&) = delete;

    [[nodiscard]] QModelIndex index(int Row, int Column, const QModelIndex& Parent = QModelIndex{}) const noexcept override {
        return Runtime::TryCatchWrapper([&]() -> QModelIndex {
            if (not hasIndex(Row, Column, Parent)) {
                return QModelIndex{};
            }
            const std::vector<std::size_t>& ChildDeckNodeIndexesVector{ GetChildDeckNodeIndexes(Parent) };
            const std::size_t ChildDeckNodeIndex{ ChildDeckNodeIndexesVector.at(static_cast<std::size_t>(Row)) };
            return createIndex(Row, Column, static_cast<quintptr>(ChildDeckNodeIndex));
        });
    }

    [[nodiscard]] QModelIndex parent(const QModelIndex& Index) const noexcept override {
        return Runtime::TryCatchWrapper([&]() -> QModelIndex {
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

    [[nodiscard]] int rowCount(const QModelIndex& Parent = QModelIndex{}) const noexcept override {
        return Runtime::TryCatchWrapper([&]() -> int {
            if (Parent.column() > 0) {
                return 0;
            }
            return static_cast<int>(GetChildDeckNodeIndexes(Parent).size());
        });
    }

    [[nodiscard]] int columnCount(const QModelIndex& Parent = QModelIndex{}) const noexcept override {
        return Runtime::TryCatchWrapper([&]() -> int {
            if (Parent.isValid() and Parent.column() > 0) {
                return 0;
            }
            return static_cast<int>(ColumnEnum::SubtreeTotalCountColumn) + 1;
        });
    }

    [[nodiscard]] QVariant data(const QModelIndex& Index, int Role) const noexcept override {
        return Runtime::TryCatchWrapper([&]() -> QVariant {
            const std::optional<std::reference_wrapper<const DeckNode>> CurrentDeckNodeOptional{ TryGetDeckNode(Index) };
            if (not CurrentDeckNodeOptional.has_value()) {
                return QVariant{};
            }
            const DeckNode& CurrentDeckNode{ CurrentDeckNodeOptional.value().get() };
            switch (Role) {
            case Qt::DisplayRole:
                switch (Index.column()) {
                case static_cast<int>(ColumnEnum::DeckNameColumn):
                    return QString::fromStdString(CurrentDeckNode.m_DeckTreeSnapshotNode.m_DeckName);
                case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
                    return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SubtreeDueNowCount;
                case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
                    return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SubtreeByTodayCount;
                case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
                    return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SubtreeTotalCount;
                default:
                    return QVariant{};
                }
            case static_cast<int>(RoleEnum::DeckIdRole):
                return QString::fromStdString(CurrentDeckNode.m_DeckTreeSnapshotNode.m_DeckId);
            case static_cast<int>(RoleEnum::ParentDeckIdRole):
                if (not CurrentDeckNode.m_DeckTreeSnapshotNode.m_ParentDeckId.has_value()) {
                    return QString{};
                }
                return QString::fromStdString(CurrentDeckNode.m_DeckTreeSnapshotNode.m_ParentDeckId.value());
            case static_cast<int>(RoleEnum::DeckNameRole):
                return QString::fromStdString(CurrentDeckNode.m_DeckTreeSnapshotNode.m_DeckName);
            case static_cast<int>(RoleEnum::CreatedAtMillisecondsSinceEpochRole):
                return CurrentDeckNode.m_DeckTreeSnapshotNode.m_CreatedAtMillisecondsSinceEpoch;
            case static_cast<int>(RoleEnum::LastUpdatedAtMillisecondsSinceEpochRole):
                if (not CurrentDeckNode.m_DeckTreeSnapshotNode.m_LastUpdatedAtMillisecondsSinceEpoch.has_value()) {
                    return std::int64_t{};
                }
                return CurrentDeckNode.m_DeckTreeSnapshotNode.m_LastUpdatedAtMillisecondsSinceEpoch.value();
            case static_cast<int>(RoleEnum::SelfDueNowCountRole):
                return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SelfDueNowCount;
            case static_cast<int>(RoleEnum::SelfByTodayCountRole):
                return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SelfByTodayCount;
            case static_cast<int>(RoleEnum::SelfTotalCountRole):
                return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SelfTotalCount;
            case static_cast<int>(RoleEnum::SubtreeDueNowCountRole):
                return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SubtreeDueNowCount;
            case static_cast<int>(RoleEnum::SubtreeByTodayCountRole):
                return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SubtreeByTodayCount;
            case static_cast<int>(RoleEnum::SubtreeTotalCountRole):
                return CurrentDeckNode.m_DeckTreeSnapshotNode.m_SubtreeTotalCount;
            case static_cast<int>(RoleEnum::TargetLanguageCodeRole):
                return static_cast<unsigned int>(CurrentDeckNode.m_DeckTreeSnapshotNode.m_TargetLanguageCode);
            default:
                return QVariant{};
            }
        });
    }

    [[nodiscard]] bool hasChildren(const QModelIndex& Parent = QModelIndex{}) const noexcept override {
        return Runtime::TryCatchWrapper([&]() -> bool {
            if (Parent.column() > 0) {
                return false;
            }
            return rowCount(Parent) > 0;
        });
    }

    void sort(int Column, Qt::SortOrder Order = Qt::AscendingOrder) noexcept override {
        Runtime::TryCatchWrapper([&]() -> void {
            if (Column < static_cast<int>(ColumnEnum::DeckNameColumn) or Column > static_cast<int>(ColumnEnum::SubtreeTotalCountColumn)) {
                return;
            }
            m_SortColumn = Column;
            m_SortOrder = Order;
            beginResetModel();
            ApplyCurrentSort();
            endResetModel();
        });
    }

    [[nodiscard]] QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const noexcept override {
        return Runtime::TryCatchWrapper([&]() -> QVariant {
            if (Orientation not_eq Qt::Horizontal or Role not_eq Qt::DisplayRole) {
                return {};
            }
            switch (Section) {
            case static_cast<int>(ColumnEnum::DeckNameColumn):
                return QStringLiteral("Deck");
            case static_cast<int>(ColumnEnum::SubtreeDueNowCountColumn):
                return QStringLiteral("Due Now");
            case static_cast<int>(ColumnEnum::SubtreeByTodayCountColumn):
                return QStringLiteral("By Today");
            case static_cast<int>(ColumnEnum::SubtreeTotalCountColumn):
                return QStringLiteral("Total");
            default:
                return {};
            }
        });
    }

    [[nodiscard]] Q_INVOKABLE QString GetColumnHeaderText(const int Column) const noexcept {
        return Runtime::TryCatchWrapper([&]() -> QString { return headerData(Column, Qt::Horizontal, Qt::DisplayRole).toString(); });
    }

    [[nodiscard]] QHash<int, QByteArray> roleNames() const noexcept override {
        return Runtime::TryCatchWrapper([&]() -> QHash<int, QByteArray> {
            return {
                {                                                     Qt::DisplayRole,                             "display" },
                {                              static_cast<int>(RoleEnum::DeckIdRole),                              "deckId" },
                {                        static_cast<int>(RoleEnum::ParentDeckIdRole),                        "parentDeckId" },
                {                            static_cast<int>(RoleEnum::DeckNameRole),                            "deckName" },
                {     static_cast<int>(RoleEnum::CreatedAtMillisecondsSinceEpochRole),     "createdAtMillisecondsSinceEpoch" },
                { static_cast<int>(RoleEnum::LastUpdatedAtMillisecondsSinceEpochRole), "lastUpdatedAtMillisecondsSinceEpoch" },
                {                     static_cast<int>(RoleEnum::SelfDueNowCountRole),                     "selfDueNowCount" },
                {                    static_cast<int>(RoleEnum::SelfByTodayCountRole),                    "selfByTodayCount" },
                {                      static_cast<int>(RoleEnum::SelfTotalCountRole),                      "selfTotalCount" },
                {                  static_cast<int>(RoleEnum::SubtreeDueNowCountRole),                  "subtreeDueNowCount" },
                {                 static_cast<int>(RoleEnum::SubtreeByTodayCountRole),                 "subtreeByTodayCount" },
                {                   static_cast<int>(RoleEnum::SubtreeTotalCountRole),                   "subtreeTotalCount" },
                {                  static_cast<int>(RoleEnum::TargetLanguageCodeRole),                  "targetLanguageCode" },
            };
        });
    }

    void ReplaceAll(std::vector<Domain::Deck::DeckTreeSnapshotNode>&&) noexcept;

private:
    struct DeckNode {
        Domain::Deck::DeckTreeSnapshotNode m_DeckTreeSnapshotNode;
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
