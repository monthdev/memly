#include "Presentation/Controller/DeckTreeController.hpp"

#include "Runtime/Crash.hpp"

namespace Presentation::Controller {

[[nodiscard]] QString DeckTreeController::GetNameLengthErrorMessage() const {
    return QString{ "Deck name length exceeds character limit" };
}

[[nodiscard]] QString DeckTreeController::GetDuplicateSiblingDeckNameErrorMessage() const {
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
    const std::optional<Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError) const {
    if (not RecoverableDeckMutationError.has_value()) {
        return std::nullopt;
    }
    switch (RecoverableDeckMutationError.value()) {
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::DeckNameLengthError: {
        return GetNameLengthErrorMessage();
    }
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::DuplicateSiblingDeckNameError: {
        return GetDuplicateSiblingDeckNameErrorMessage();
    }
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::InvalidTargetLanguageCodeError: {
        return GetTargetLanguageCodeErrorMessage();
    }
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError: {
        return GetParentDeckTargetLanguageMismatchErrorMessage();
    }
    case Infrastructure::Store::Deck::DeckStore::RecoverableDeckMutationErrorEnum::DeckTreeCycleDetectionError: {
        return GetDeckTreeCycleDetectionErrorMessage();
    }
    }
}

[[nodiscard]] std::optional<QString> DeckTreeController::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        if (m_DeckTree.WouldCreateRootDeckDuplicateSiblingName(DeckName)) {
            return GetDuplicateSiblingDeckNameErrorMessage();
        }
        const std::optional<QString> RecoverableDeckMutationErrorMessage{ RecoverableDeckMutationErrorToQString(
            m_DeckStore.CreateRootDeck(DeckName, TargetLanguageCode)) };
        if (not RecoverableDeckMutationErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return RecoverableDeckMutationErrorMessage;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        if (m_DeckTree.WouldCreateChildDeckDuplicateSiblingName(DeckName, ParentDeckId)) {
            return GetDuplicateSiblingDeckNameErrorMessage();
        }
        const std::optional<QString> RecoverableDeckMutationErrorMessage{ RecoverableDeckMutationErrorToQString(
            m_DeckStore.CreateChildDeck(DeckName, ParentDeckId)) };
        if (not RecoverableDeckMutationErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return RecoverableDeckMutationErrorMessage;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::MoveDeck(const QString& DeckId, const std::optional<QString>& NewParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        if (m_DeckTree.WouldReparentCreateCycle(DeckId, NewParentDeckId)) {
            return GetDeckTreeCycleDetectionErrorMessage();
        }
        if (m_DeckTree.WouldReparentCreateTargetLanguageMismatch(DeckId, NewParentDeckId)) {
            return GetParentDeckTargetLanguageMismatchErrorMessage();
        }
        if (m_DeckTree.WouldMoveDeckDuplicateSiblingName(DeckId, NewParentDeckId)) {
            return GetDuplicateSiblingDeckNameErrorMessage();
        }
        const std::optional<QString> RecoverableDeckMutationErrorMessage{ RecoverableDeckMutationErrorToQString(
            m_DeckStore.MoveDeck(DeckId, NewParentDeckId)) };
        if (not RecoverableDeckMutationErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return RecoverableDeckMutationErrorMessage;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        if (m_DeckTree.WouldRenameDeckDuplicateSiblingName(DeckId, NewDeckName)) {
            return GetDuplicateSiblingDeckNameErrorMessage();
        }
        const std::optional<QString> RecoverableDeckMutationErrorMessage{ RecoverableDeckMutationErrorToQString(m_DeckStore.RenameDeck(DeckId, NewDeckName)) };
        if (not RecoverableDeckMutationErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        }
        return RecoverableDeckMutationErrorMessage;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::DeleteDeck(const QString& DeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::optional<QString> RecoverableDeckMutationErrorMessage{ RecoverableDeckMutationErrorToQString(m_DeckStore.DeleteDeck(DeckId)) };
        if (not RecoverableDeckMutationErrorMessage.has_value()) {
            m_LibraryRefreshCoordinator.NotifyLibraryMutated(true);
        }
        return RecoverableDeckMutationErrorMessage;
    });
}

void DeckTreeController::RefreshDeckTree(const qint64 AsOfMillisecondsSinceEpoch) noexcept {
    Runtime::TryCatchWrapper([&] {
        const QVector<Infrastructure::Store::Deck::DeckTreeStore::DeckTreeRow> DeckTreeRowQVector{ m_DeckTreeStore.ReadDeckTreeSnapshot(
            AsOfMillisecondsSinceEpoch) };
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
