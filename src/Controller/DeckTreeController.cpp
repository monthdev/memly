#include "Controller/DeckTreeController.hpp"

#include <cstdint>

#include "Support/Fatal.hpp"

namespace Controller {

[[nodiscard]] Q_INVOKABLE QString DeckTreeController::NameLengthErrorMessage() const {
    return QString{ "Deck name length exceeds character limit" };
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeController::DuplicateNameErrorMessage() const {
    return QString{ "Deck name already exists in this parent deck" };
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeController::TargetLanguageCodeErrorMessage() const {
    return QString{ "Target language code is invalid" };
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeController::ParentDeckErrorMessage() const {
    return QString{ "Parent deck does not exist" };
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeController::CycleDetectionErrorMessage() const {
    return QString{ "Deck cannot be moved into itself or one of its child decks" };
}

[[nodiscard]] QString DeckTreeController::HandleDeckMutationStatus(
    const Store::DeckHierarchyStore::DeckMutationStatus DeckMutationStatus) const {
    switch (DeckMutationStatus) {
    case Store::DeckHierarchyStore::DeckMutationStatus::Success: {
        return QString{};
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::NameLengthError: {
        return NameLengthErrorMessage();
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::DuplicateNameError: {
        return DuplicateNameErrorMessage();
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::TargetLanguageCodeError: {
        return TargetLanguageCodeErrorMessage();
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::ParentDeckError: {
        return ParentDeckErrorMessage();
    }
    case Store::DeckHierarchyStore::DeckMutationStatus::CycleDetectionError: {
        return CycleDetectionErrorMessage();
    }
    default:
        Support::ThrowError("Unknown deck mutation status failure");
    }
}

[[nodiscard]] Q_INVOKABLE QString
DeckTreeController::CreateDeck(const QString& DeckName,
                               const quint8 TargetLanguageCode,
                               const QString& ParentDeckId) noexcept {
    return Support::TryCatchWrapper([&] {
        const QString ErrorMessage{ HandleDeckMutationStatus(
            m_DeckHierarchyStore.CreateDeck(DeckName, TargetLanguageCode, ParentDeckId)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTree(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString
DeckTreeController::MoveDeck(const QString& DeckId, const QString& ParentDeckId) noexcept {
    return Support::TryCatchWrapper([&] {
        const QString ErrorMessage{ HandleDeckMutationStatus(
            m_DeckHierarchyStore.MoveDeck(DeckId, ParentDeckId)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTree(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString
DeckTreeController::UpdateDeckName(const QString& DeckId, const QString& DeckName) noexcept {
    return Support::TryCatchWrapper([&] {
        const QString ErrorMessage{ HandleDeckMutationStatus(
            m_DeckHierarchyStore.UpdateDeckName(DeckId, DeckName)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckTree(false);
        }
        return ErrorMessage;
    });
}

Q_INVOKABLE void DeckTreeController::OnActivated() {
    RefreshDeckTree(true);
}

Q_INVOKABLE void DeckTreeController::OnDeactivated() {
    m_DeckTreeRefreshTimer.stop();
}

Q_INVOKABLE void DeckTreeController::DeleteDeck(const QString& DeckId) noexcept {
    Support::TryCatchWrapper([&] {
        m_DeckHierarchyStore.DeleteDeck(DeckId);
        RefreshDeckTree(true);
    });
}

void DeckTreeController::ScheduleNextDeckTreeRefresh() noexcept {
    Support::TryCatchWrapper([&] {
        m_DeckTreeRefreshTimer.stop();
        const std::optional<std::int64_t> DeckHierarchyViewNextRefreshDelayMilliseconds{
            m_DeckHierarchyStore.ReadDeckHierarchyViewNextRefreshDelayMilliseconds()
        };
        if (!DeckHierarchyViewNextRefreshDelayMilliseconds.has_value()) {
            return;
        }
        m_DeckTreeRefreshTimer.start(
            static_cast<int>(DeckHierarchyViewNextRefreshDelayMilliseconds.value()));
    });
}

void DeckTreeController::RefreshDeckTree(bool NeedNextDeckTreeRefreshScheduled) noexcept {
    Support::TryCatchWrapper([&] {
        const QVector<Store::DeckHierarchyStore::DeckHierarchyRow> DeckHierarchyRowVector{
            m_DeckHierarchyStore.ReadDeckHierarchyView()
        };
        QVector<Model::DeckTreeModel::DeckTreeRow> DeckTree;
        DeckTree.reserve(DeckHierarchyRowVector.size());
        for (const auto& DeckHierarchyRow : DeckHierarchyRowVector) {
            DeckTree.emplace_back(DeckHierarchyRow.m_Id,
                                  DeckHierarchyRow.m_ParentId,
                                  DeckHierarchyRow.m_Name,
                                  DeckHierarchyRow.m_DueNow,
                                  DeckHierarchyRow.m_ByToday,
                                  DeckHierarchyRow.m_Total);
        }
        m_DeckTree.ReplaceAll(std::move(DeckTree));
        if (NeedNextDeckTreeRefreshScheduled) {
            ScheduleNextDeckTreeRefresh();
        }
    });
}
}
