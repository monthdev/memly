#include "Presentation/Controller/DeckTreeController.hpp"

#include <expected>

#include "Runtime/Crash.hpp"

namespace Presentation::Controller {

[[nodiscard]] QString
DeckTreeController::DeckMutationErrorToQString(const Application::Service::Deck::DeckService::DeckMutationErrorEnum DeckMutationError) const {
    switch (DeckMutationError) {
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::DeckNameLengthError: {
        return QString{ "Deck name length exceeds character limit" };
    }
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::DuplicateSiblingDeckNameError: {
        return QString{ "Deck name already exists" };
    }
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::InvalidTargetLanguageCodeError: {
        return QString{ "Target language code is invalid" };
    }
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError: {
        return QString{ "Deck target language does not match parent deck" };
    }
    case Application::Service::Deck::DeckService::DeckMutationErrorEnum::DeckTreeCycleDetectionError: {
        return QString{ "Deck cannot be moved into itself or one of its own sub decks" };
    }
    }
}

[[nodiscard]] std::optional<QString> DeckTreeController::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.CreateRootDeck(
            DeckName, TargetLanguageCode) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.CreateChildDeck(
            DeckName, ParentDeckId) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::MoveDeck(const QString& DeckId, const std::optional<QString>& NewParentDeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.MoveDeck(DeckId,
                                                                                                                                             NewParentDeckId) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.RenameDeck(DeckId,
                                                                                                                                               NewDeckName) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        m_LibraryRefreshCoordinator.NotifyLibraryMutated(false);
        return std::nullopt;
    });
}

[[nodiscard]] std::optional<QString> DeckTreeController::DeleteDeck(const QString& DeckId) noexcept {
    return Runtime::TryCatchWrapper([&]() -> std::optional<QString> {
        const std::expected<void, Application::Service::Deck::DeckService::DeckMutationErrorEnum> DeckMutationResult{ m_DeckService.DeleteDeck(DeckId) };
        if (not DeckMutationResult.has_value()) {
            return DeckMutationErrorToQString(DeckMutationResult.error());
        }
        m_LibraryRefreshCoordinator.NotifyLibraryMutated(true);
        return std::nullopt;
    });
}

void DeckTreeController::RefreshDeckTree(const qint64 AsOfMillisecondsSinceEpoch) noexcept {
    Runtime::TryCatchWrapper([&]() -> void {
        const QVector<Application::Service::Deck::DeckTreeService::DeckTreeRow> DeckTreeRowQVector{ m_DeckTreeService.ReadDeckTreeSnapshot(
            AsOfMillisecondsSinceEpoch) };
        QVector<Model::DeckTreeModel::DeckNodeData> DeckNodeDataQVector{};
        DeckNodeDataQVector.reserve(DeckTreeRowQVector.size());
        for (const Application::Service::Deck::DeckTreeService::DeckTreeRow& DeckTreeRow : DeckTreeRowQVector) {
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
