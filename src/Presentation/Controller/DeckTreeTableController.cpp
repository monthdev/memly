#include "Presentation/Controller/DeckTreeTableController.hpp"

#include <QDateTime>
#include <algorithm>
#include <cstdint>

#include "Support/Fatal.hpp"

namespace Presentation::Controller {

[[nodiscard]] QString DeckTreeTableController::GetNameLengthErrorMessage() const {
    return QString{ "Deck name length exceeds character limit" };
}

[[nodiscard]] QString DeckTreeTableController::GetDuplicateNameErrorMessage() const {
    return QString{ "Deck name already exists" };
}

[[nodiscard]] QString DeckTreeTableController::GetTargetLanguageCodeErrorMessage() const {
    return QString{ "Target language code is invalid" };
}

[[nodiscard]] QString DeckTreeTableController::GetParentDeckErrorMessage() const {
    return QString{ "Parent deck does not exist" };
}

[[nodiscard]] QString DeckTreeTableController::GetCycleDetectionErrorMessage() const {
    return QString{ "Deck cannot be moved into itself or one of its own sub decks" };
}

[[nodiscard]] QString DeckTreeTableController::HandleDeckMutationStatus(const Infrastructure::Store::DeckStore::DeckMutationStatus DeckMutationStatus) const {
    switch (DeckMutationStatus) {
    case Infrastructure::Store::DeckStore::DeckMutationStatus::Success: {
        return QString{};
    }
    case Infrastructure::Store::DeckStore::DeckMutationStatus::NameLengthError: {
        return GetNameLengthErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationStatus::DuplicateNameError: {
        return GetDuplicateNameErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationStatus::TargetLanguageCodeError: {
        return GetTargetLanguageCodeErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationStatus::ParentDeckError: {
        return GetParentDeckErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationStatus::CycleDetectionError: {
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
        if (m_DeckTreeTable.WouldCreateDeckHaveDuplicateSiblingName(DeckName, ParentDeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationStatus(m_DeckStore.CreateDeck(DeckName, TargetLanguageCode, ParentDeckId)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTreeTable(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::MoveDeck(const QString& DeckId, const QString& NewParentDeckId) noexcept {
    return Support::TryCatchWrapper([&] {
        if (m_DeckTreeTable.WouldMoveDeckCreateCycle(DeckId, NewParentDeckId)) {
            return GetCycleDetectionErrorMessage();
        }
        if (m_DeckTreeTable.WouldMoveDeckHaveDuplicateSiblingName(DeckId, NewParentDeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationStatus(m_DeckStore.MoveDeck(DeckId, NewParentDeckId)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTreeTable(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::UpdateDeckName(const QString& DeckId, const QString& DeckName) noexcept {
    return Support::TryCatchWrapper([&] {
        if (m_DeckTreeTable.WouldUpdateDeckNameHaveDuplicateSiblingName(DeckId, DeckName)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationStatus(m_DeckStore.UpdateDeckName(DeckId, DeckName)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTreeTable(false);
        }
        return ErrorMessage;
    });
}

Q_INVOKABLE void DeckTreeTableController::OnActivated() noexcept {
    RefreshDeckTreeTable(true);
}

Q_INVOKABLE void DeckTreeTableController::OnDeactivated() {
    m_DeckTreeRefreshQTimer.stop();
}

Q_INVOKABLE void DeckTreeTableController::DeleteDeck(const QString& DeckId) noexcept {
    Support::TryCatchWrapper([&] {
        m_DeckStore.DeleteDeck(DeckId);
        RefreshDeckTreeTable(true);
    });
}

void DeckTreeTableController::ScheduleNextDeckTreeTableRefresh(const std::optional<std::int64_t>& NextRefreshAtMillisecondsSinceEpoch) {
    m_DeckTreeRefreshQTimer.stop();
    if (not NextRefreshAtMillisecondsSinceEpoch.has_value()) {
        return;
    }
    const std::int64_t NextRefreshDelayMilliseconds{ std::max<std::int64_t>(
        0, NextRefreshAtMillisecondsSinceEpoch.value() - QDateTime::currentMSecsSinceEpoch()) };
    m_DeckTreeRefreshQTimer.start(static_cast<int>(NextRefreshDelayMilliseconds));
}

void DeckTreeTableController::RefreshDeckTreeTable(bool NeedNextDeckTreeTableRefreshScheduled) noexcept {
    Support::TryCatchWrapper([&] {
        const Infrastructure::Store::DeckHierarchyStore::DeckHierarchyViewSnapshot DeckHierarchyViewSnapshot{ m_DeckHierarchyStore.ReadDeckHierarchyViewSnapshot() };
        QVector<Model::DeckTreeTableModel::DeckNodeData> DeckNodeDataQVector;
        DeckNodeDataQVector.reserve(DeckHierarchyViewSnapshot.m_DeckHierarchyRowQVector.size());
        for (const auto& DeckHierarchyRow : DeckHierarchyViewSnapshot.m_DeckHierarchyRowQVector) {
            DeckNodeDataQVector.emplace_back(DeckHierarchyRow.m_Id,
                                             DeckHierarchyRow.m_ParentId,
                                             DeckHierarchyRow.m_Name,
                                             DeckHierarchyRow.m_DueNowCount,
                                             DeckHierarchyRow.m_ByTodayCount,
                                             DeckHierarchyRow.m_TotalCount,
                                             DeckHierarchyRow.m_TargetLanguageCode);
        }
        m_DeckTreeTable.ReplaceAll(std::move(DeckNodeDataQVector));
        if (NeedNextDeckTreeTableRefreshScheduled) {
            ScheduleNextDeckTreeTableRefresh(DeckHierarchyViewSnapshot.m_NextRefreshAtMillisecondsSinceEpoch);
        }
    });
}
}
