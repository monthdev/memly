#include "Controller/DeckTreeTableController.hpp"

#include <QDateTime>
#include <algorithm>
#include <cstdint>

#include "Support/Fatal.hpp"

namespace Controller {

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::GetNameLengthErrorMessage() const {
    return QString{ "Deck name length exceeds character limit" };
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::GetDuplicateNameErrorMessage() const {
    return QString{ "Deck name already exists" };
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::GetTargetLanguageCodeErrorMessage() const {
    return QString{ "Target language code is invalid" };
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::GetParentDeckErrorMessage() const {
    return QString{ "Parent deck does not exist" };
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::GetCycleDetectionErrorMessage() const {
    return QString{ "Deck cannot be moved into itself or one of its own sub decks" };
}

[[nodiscard]] QString DeckTreeTableController::HandleDeckMutationStatus(const Store::DeckHierarchyStore::DeckMutationStatus DeckMutationStatus) const {
    switch (DeckMutationStatus) {
    case Store::DeckHierarchyStore::DeckMutationStatus::Success: {
        return QString{};
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::NameLengthError: {
        return GetNameLengthErrorMessage();
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::DuplicateNameError: {
        return GetDuplicateNameErrorMessage();
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::TargetLanguageCodeError: {
        return GetTargetLanguageCodeErrorMessage();
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::ParentDeckError: {
        return GetParentDeckErrorMessage();
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::CycleDetectionError: {
        return GetCycleDetectionErrorMessage();
    }
    default:
        Support::ThrowError("Unknown deck mutation status failure");
    }
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::CreateDeck(const QString& DeckName,
                                                                      const quint8 TargetLanguageCode,
                                                                      const QString& ParentDeckId) noexcept {
    return Support::TryCatchWrapper([&] {
        if (m_DeckTree.WouldCreateDeckHaveDuplicateSiblingName(DeckName, ParentDeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationStatus(m_DeckHierarchyStore.CreateDeck(DeckName, TargetLanguageCode, ParentDeckId)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTree(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::MoveDeck(const QString& DeckId, const QString& ParentDeckId) noexcept {
    return Support::TryCatchWrapper([&] {
        if (m_DeckTree.WouldMoveDeckCreateCycle(DeckId, ParentDeckId)) {
            return GetCycleDetectionErrorMessage();
        }
        if (m_DeckTree.WouldMoveDeckHaveDuplicateSiblingName(DeckId, ParentDeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationStatus(m_DeckHierarchyStore.MoveDeck(DeckId, ParentDeckId)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTree(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::UpdateDeckName(const QString& DeckId, const QString& DeckName) noexcept {
    return Support::TryCatchWrapper([&] {
        if (m_DeckTree.WouldUpdateDeckNameHaveDuplicateSiblingName(DeckId, DeckName)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationStatus(m_DeckHierarchyStore.UpdateDeckName(DeckId, DeckName)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTree(false);
        }
        return ErrorMessage;
    });
}

Q_INVOKABLE void DeckTreeTableController::OnActivated() noexcept {
    RefreshDeckTree(true);
}

Q_INVOKABLE void DeckTreeTableController::OnDeactivated() {
    m_DeckTreeRefreshTimer.stop();
}

Q_INVOKABLE void DeckTreeTableController::DeleteDeck(const QString& DeckId) noexcept {
    Support::TryCatchWrapper([&] {
        m_DeckHierarchyStore.DeleteDeck(DeckId);
        RefreshDeckTree(true);
    });
}

void DeckTreeTableController::ScheduleNextDeckTreeRefresh(const std::optional<std::int64_t>& NextRefreshAtMillisecondsSinceEpoch) {
    m_DeckTreeRefreshTimer.stop();
    if (not NextRefreshAtMillisecondsSinceEpoch.has_value()) {
        return;
    }
    const std::int64_t NextRefreshDelayMilliseconds{ std::max<std::int64_t>(
        0, NextRefreshAtMillisecondsSinceEpoch.value() - QDateTime::currentMSecsSinceEpoch()) };
    m_DeckTreeRefreshTimer.start(static_cast<int>(NextRefreshDelayMilliseconds));
}

void DeckTreeTableController::RefreshDeckTree(bool NeedNextDeckTreeRefreshScheduled) noexcept {
    Support::TryCatchWrapper([&] {
        const Store::DeckHierarchyStore::DeckHierarchyViewSnapshot DeckHierarchyViewSnapshot{ m_DeckHierarchyStore.ReadDeckHierarchyViewSnapshot() };
        QVector<Model::DeckTreeTableModel::DeckNodeData> DeckNodeDataQVector;
        DeckNodeDataQVector.reserve(DeckHierarchyViewSnapshot.m_DeckHierarchyRowQVector.size());
        for (const auto& DeckHierarchyRow : DeckHierarchyViewSnapshot.m_DeckHierarchyRowQVector) {
            DeckNodeDataQVector.emplace_back(DeckHierarchyRow.m_Id,
                                             DeckHierarchyRow.m_ParentId,
                                             DeckHierarchyRow.m_Name,
                                             DeckHierarchyRow.m_DueNow,
                                             DeckHierarchyRow.m_ByToday,
                                             DeckHierarchyRow.m_Total,
                                             DeckHierarchyRow.m_TargetLanguageCode);
        }
        m_DeckTree.ReplaceAll(std::move(DeckNodeDataQVector));
        if (NeedNextDeckTreeRefreshScheduled) {
            ScheduleNextDeckTreeRefresh(DeckHierarchyViewSnapshot.m_NextRefreshAtMillisecondsSinceEpoch);
        }
    });
}
}
