#include "Presentation/Controller/DeckTreeTableController.hpp"

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

[[nodiscard]] QString DeckTreeTableController::GetParentDeckTargetLanguageMismatchErrorMessage() const {
    return QString{ "Deck target language does not match parent deck" };
}

[[nodiscard]] QString DeckTreeTableController::GetCycleDetectionErrorMessage() const {
    return QString{ "Deck cannot be moved into itself or one of its own sub decks" };
}

[[nodiscard]] QString
DeckTreeTableController::HandleDeckMutationError(const std::optional<Infrastructure::Store::DeckStore::DeckMutationErrorEnum> DeckMutationError) const {
    if (not DeckMutationError.has_value()) {
        return QString{};
    }
    switch (DeckMutationError.value()) {
    case Infrastructure::Store::DeckStore::DeckMutationErrorEnum::NameLengthError: {
        return GetNameLengthErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationErrorEnum::DuplicateNameError: {
        return GetDuplicateNameErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationErrorEnum::TargetLanguageCodeError: {
        return GetTargetLanguageCodeErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationErrorEnum::ParentDeckError: {
        return GetParentDeckErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError: {
        return GetParentDeckTargetLanguageMismatchErrorMessage();
    }
    case Infrastructure::Store::DeckStore::DeckMutationErrorEnum::CycleDetectionError: {
        return GetCycleDetectionErrorMessage();
    }
    default:
        Support::ThrowError("Unknown deck mutation error failure");
    }
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return Support::TryCatchWrapper([&] {
        if (m_DeckTreeTable.HasDuplicateSiblingName(DeckName, QString{})) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationError(m_DeckStore.CreateRootDeck(DeckName, TargetLanguageCode)) };
        if (ErrorMessage.isEmpty()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return Support::TryCatchWrapper([&] {
        if (m_DeckTreeTable.HasDuplicateSiblingName(DeckName, ParentDeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationError(m_DeckStore.CreateChildDeck(DeckName, ParentDeckId)) };
        if (ErrorMessage.isEmpty()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::MoveDeck(const QString& DeckId, const QString& NewParentDeckId) noexcept {
    return Support::TryCatchWrapper([&] {
        if (m_DeckTreeTable.WouldReparentCreateCycle(DeckId, NewParentDeckId)) {
            return GetCycleDetectionErrorMessage();
        }
        if (m_DeckTreeTable.WouldReparentCreateTargetLanguageMismatch(DeckId, NewParentDeckId)) {
            return GetParentDeckTargetLanguageMismatchErrorMessage();
        }
        const auto CurrentDeckNodeData{ m_DeckTreeTable.TryGetDeckNodeData(DeckId) };
        if (CurrentDeckNodeData.has_value() and m_DeckTreeTable.HasDuplicateSiblingName(CurrentDeckNodeData->get().m_DeckName, NewParentDeckId, DeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationError(m_DeckStore.MoveDeck(DeckId, NewParentDeckId)) };
        if (ErrorMessage.isEmpty()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString DeckTreeTableController::UpdateDeckName(const QString& DeckId, const QString& NewDeckName) noexcept {
    return Support::TryCatchWrapper([&] {
        const auto CurrentDeckNodeData{ m_DeckTreeTable.TryGetDeckNodeData(DeckId) };
        if (CurrentDeckNodeData.has_value() and m_DeckTreeTable.HasDuplicateSiblingName(NewDeckName, CurrentDeckNodeData->get().m_ParentDeckId, DeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const QString ErrorMessage{ HandleDeckMutationError(m_DeckStore.UpdateDeckName(DeckId, NewDeckName)) };
        if (ErrorMessage.isEmpty()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return ErrorMessage;
    });
}

Q_INVOKABLE void DeckTreeTableController::DeleteDeck(const QString& DeckId) noexcept {
    Support::TryCatchWrapper([&] {
        m_DeckStore.DeleteDeck(DeckId);
        m_LibraryRefreshCoordinator.NotifyLibraryMutated(true);
    });
}

void DeckTreeTableController::RefreshDeckTreeTable(const qint64 AsOfMillisecondsSinceEpoch) noexcept {
    Support::TryCatchWrapper([&] {
        const QVector<Infrastructure::Store::DeckTreeStore::DeckTreeRow> DeckTreeRowQVector{ m_DeckTreeStore.ReadDeckTreeSnapshot(AsOfMillisecondsSinceEpoch) };
        QVector<Model::DeckTreeTableModel::DeckNodeData> DeckNodeDataQVector{};
        DeckNodeDataQVector.reserve(DeckTreeRowQVector.size());
        for (const auto& DeckTreeRow : DeckTreeRowQVector) {
            DeckNodeDataQVector.emplace_back(DeckTreeRow.m_DeckId,
                                             DeckTreeRow.m_ParentDeckId,
                                             DeckTreeRow.m_DeckName,
                                             DeckTreeRow.m_DueNowCount,
                                             DeckTreeRow.m_ByTodayCount,
                                             DeckTreeRow.m_TotalCount,
                                             DeckTreeRow.m_TargetLanguageCode);
        }
        m_DeckTreeTable.ReplaceAll(std::move(DeckNodeDataQVector));
    });
}
}
