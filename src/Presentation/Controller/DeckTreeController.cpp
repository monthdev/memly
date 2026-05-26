#include "Presentation/Controller/DeckTreeController.hpp"

#include "Support/Fatal.hpp"

namespace Presentation::Controller {

[[nodiscard]] QString DeckTreeController::GetNameLengthErrorMessage() const {
    return QString{ "Deck name length exceeds character limit" };
}

[[nodiscard]] QString DeckTreeController::GetDuplicateNameErrorMessage() const {
    return QString{ "Deck name already exists" };
}

[[nodiscard]] QString DeckTreeController::GetTargetLanguageCodeErrorMessage() const {
    return QString{ "Target language code is invalid" };
}

[[nodiscard]] QString DeckTreeController::GetParentDeckTargetLanguageMismatchErrorMessage() const {
    return QString{ "Deck target language does not match parent deck" };
}

[[nodiscard]] QString DeckTreeController::GetDeckTreeCycleDetectionErrorMessage() const {
    return QString{ "Deck cannot be moved into itself or one of its own sub decks" };
}

[[nodiscard]] std::optional<QString> DeckTreeController::RecoverableDeckMutationErrorToQString(
    const std::optional<Infrastructure::Store::DeckStore::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError) const {
    if (not RecoverableDeckMutationError.has_value()) {
        return std::nullopt;
    }
    switch (RecoverableDeckMutationError.value()) {
    case Infrastructure::Store::DeckStore::RecoverableDeckMutationErrorEnum::DeckNameLengthError: {
        return GetNameLengthErrorMessage();
    }
    case Infrastructure::Store::DeckStore::RecoverableDeckMutationErrorEnum::DuplicateDeckNameError: {
        return GetDuplicateNameErrorMessage();
    }
    case Infrastructure::Store::DeckStore::RecoverableDeckMutationErrorEnum::InvalidTargetLanguageCodeError: {
        return GetTargetLanguageCodeErrorMessage();
    }
    case Infrastructure::Store::DeckStore::RecoverableDeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError: {
        return GetParentDeckTargetLanguageMismatchErrorMessage();
    }
    case Infrastructure::Store::DeckStore::RecoverableDeckMutationErrorEnum::DeckTreeCycleDetectionError: {
        return GetDeckTreeCycleDetectionErrorMessage();
    }
    default:
        Support::ThrowError("Unknown recoverable deck mutation error");
    }
}

[[nodiscard]] std::optional<QString> DeckTreeController::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return Support::TryCatchWrapper([&]() -> std::optional<QString> {
        if (m_DeckTree.WouldCreateRootDeckDuplicateSiblingName(DeckName)) {
            return GetDuplicateNameErrorMessage();
        }
        const std::optional<QString> ErrorMessage{ RecoverableDeckMutationErrorToQString(m_DeckStore.CreateRootDeck(DeckName, TargetLanguageCode)) };
        if (not ErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return Support::TryCatchWrapper([&]() -> std::optional<QString> {
        if (m_DeckTree.WouldCreateChildDeckDuplicateSiblingName(DeckName, ParentDeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const std::optional<QString> ErrorMessage{ RecoverableDeckMutationErrorToQString(m_DeckStore.CreateChildDeck(DeckName, ParentDeckId)) };
        if (not ErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::MoveDeck(const QString& DeckId, const std::optional<QString>& NewParentDeckId) noexcept {
    return Support::TryCatchWrapper([&]() -> std::optional<QString> {
        if (m_DeckTree.WouldReparentCreateCycle(DeckId, NewParentDeckId)) {
            return GetDeckTreeCycleDetectionErrorMessage();
        }
        if (m_DeckTree.WouldReparentCreateTargetLanguageMismatch(DeckId, NewParentDeckId)) {
            return GetParentDeckTargetLanguageMismatchErrorMessage();
        }
        if (m_DeckTree.WouldMoveDeckDuplicateSiblingName(DeckId, NewParentDeckId)) {
            return GetDuplicateNameErrorMessage();
        }
        const std::optional<QString> ErrorMessage{ RecoverableDeckMutationErrorToQString(m_DeckStore.MoveDeck(DeckId, NewParentDeckId)) };
        if (not ErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
    return Support::TryCatchWrapper([&]() -> std::optional<QString> {
        if (m_DeckTree.WouldRenameDeckDuplicateSiblingName(DeckId, NewDeckName)) {
            return GetDuplicateNameErrorMessage();
        }
        const std::optional<QString> ErrorMessage{ RecoverableDeckMutationErrorToQString(m_DeckStore.RenameDeck(DeckId, NewDeckName)) };
        if (not ErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::DeleteDeck(const QString& DeckId) noexcept {
    return Support::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::optional<QString> ErrorMessage{ RecoverableDeckMutationErrorToQString(m_DeckStore.DeleteDeck(DeckId)) };
        if (not ErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(true);
        }
        return ErrorMessage;
    });
}

void DeckTreeController::RefreshDeckTree(const qint64 AsOfMillisecondsSinceEpoch) noexcept {
    Support::TryCatchWrapper([&] {
        const QVector<Infrastructure::Store::DeckTreeStore::DeckTreeRow> DeckTreeRowQVector{ m_DeckTreeStore.ReadDeckTreeSnapshot(AsOfMillisecondsSinceEpoch) };
        QVector<Model::DeckTreeModel::DeckNodeData> DeckNodeDataQVector{};
        DeckNodeDataQVector.reserve(DeckTreeRowQVector.size());
        for (const auto& DeckTreeRow : DeckTreeRowQVector) {
            DeckNodeDataQVector.emplace_back(DeckTreeRow.m_DeckId,
                                             DeckTreeRow.m_ParentDeckId,
                                             DeckTreeRow.m_DeckName,
                                             DeckTreeRow.m_CreatedAtMillisecondsSinceEpoch,
                                             DeckTreeRow.m_UpdatedAtMillisecondsSinceEpoch,
                                             DeckTreeRow.m_SelfDueNowCount,
                                             DeckTreeRow.m_SelfByTodayCount,
                                             DeckTreeRow.m_SelfTotalCount,
                                             DeckTreeRow.m_SubtreeDueNowCount,
                                             DeckTreeRow.m_SubtreeByTodayCount,
                                             DeckTreeRow.m_SubtreeTotalCount,
                                             DeckTreeRow.m_TargetLanguageCode);
        }
        m_DeckTree.ReplaceAll(std::move(DeckNodeDataQVector));
    });
}
}
