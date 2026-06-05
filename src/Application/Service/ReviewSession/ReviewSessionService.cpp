#include "Application/Service/ReviewSession/ReviewSessionService.hpp"

#include <optional>

#include "Infrastructure/Sql/TransactionRunner.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

namespace {

[[nodiscard]] Application::Service::ReviewSession::ReviewSessionService::ReviewSessionMutationErrorEnum ToReviewSessionMutationError(
    const Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum RecoverableReviewSessionMutationError) noexcept {
    switch (RecoverableReviewSessionMutationError) {
    case Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum::ReviewSessionNameLengthError:
        return Application::Service::ReviewSession::ReviewSessionService::ReviewSessionMutationErrorEnum::ReviewSessionNameLengthError;
    case Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError:
        return Application::Service::ReviewSession::ReviewSessionService::ReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError;
    case Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckSelfSelectionError:
        return Application::Service::ReviewSession::ReviewSessionService::ReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckSelfSelectionError;
    case Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckSubtreeSelectionError:
        return Application::Service::ReviewSession::ReviewSessionService::ReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckSubtreeSelectionError;
    case Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckIncludeSelectionError:
        return Application::Service::ReviewSession::ReviewSessionService::ReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckIncludeSelectionError;
    case Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckExcludeSelectionError:
        return Application::Service::ReviewSession::ReviewSessionService::ReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckExcludeSelectionError;
    }
}

[[nodiscard]] Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum ToStoreReviewSessionDeckSelectionType(
    const Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelectionTypeEnum ReviewSessionDeckSelectionType) noexcept {
    switch (ReviewSessionDeckSelectionType) {
    case Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelectionTypeEnum::Self:
        return Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum::Self;
    case Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelectionTypeEnum::Subtree:
        return Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum::Subtree;
    case Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelectionTypeEnum::ExcludeSelf:
        return Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum::ExcludeSelf;
    case Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelectionTypeEnum::ExcludeSubtree:
        return Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum::ExcludeSubtree;
    }
}

[[nodiscard]] QVector<Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelection> ToStoreReviewSessionDeckSelectionQVector(
    const QVector<Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionQVector) {
    QVector<Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelection> StoreReviewSessionDeckSelectionQVector{};
    StoreReviewSessionDeckSelectionQVector.reserve(ReviewSessionDeckSelectionQVector.size());
    for (const Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelection& ReviewSessionDeckSelection :
         ReviewSessionDeckSelectionQVector) {
        StoreReviewSessionDeckSelectionQVector.emplace_back(ReviewSessionDeckSelection.m_DeckId,
                                                            ToStoreReviewSessionDeckSelectionType(ReviewSessionDeckSelection.m_DeckSelectionType));
    }
    return StoreReviewSessionDeckSelectionQVector;
}

}

namespace Application::Service::ReviewSession {

ReviewSessionService::ReviewSessionService(Infrastructure::Sql::TransactionRunner& TransactionRunner,
                                           Infrastructure::Store::ReviewSession::ReviewSessionStore& ReviewSessionStore) noexcept
    : m_TransactionRunner{ TransactionRunner }
    , m_ReviewSessionStore{ ReviewSessionStore } {
}

[[nodiscard]] std::expected<QString, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::CreateOrReadExistingDefaultReviewSession(const QString& RootDeckId, const QString& ReviewSessionDefinitionKey) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<QString, ReviewSessionMutationErrorEnum> {
        std::expected<QString, Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum> StoreReviewSessionId{
            m_ReviewSessionStore.CreateOrReadExistingDefaultReviewSession(RootDeckId, ReviewSessionDefinitionKey)
        };
        if (not StoreReviewSessionId.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(StoreReviewSessionId.error()) };
        }
        return StoreReviewSessionId.value();
    });
}

[[nodiscard]] std::expected<QString, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::CreateOrReadExistingCustomReviewSession(const QString& ReviewSessionName,
                                                              const QString& ReviewSessionDefinitionKey,
                                                              const QVector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionQVector) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<QString, ReviewSessionMutationErrorEnum> {
        const QVector<Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelection> StoreReviewSessionDeckSelectionQVector{
            ToStoreReviewSessionDeckSelectionQVector(ReviewSessionDeckSelectionQVector)
        };
        std::expected<QString, Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum> StoreReviewSessionId{
            m_ReviewSessionStore.CreateOrReadExistingCustomReviewSession(ReviewSessionName, ReviewSessionDefinitionKey, StoreReviewSessionDeckSelectionQVector)
        };
        if (not StoreReviewSessionId.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(StoreReviewSessionId.error()) };
        }
        return StoreReviewSessionId.value();
    });
}

[[nodiscard]] std::expected<void, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::RenameReviewSession(const QString& ReviewSessionId, const QString& ReviewSessionName) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, ReviewSessionMutationErrorEnum> {
        const std::optional<Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
            RecoverableReviewSessionMutationError{ m_ReviewSessionStore.RenameReviewSession(ReviewSessionId, ReviewSessionName) };
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(RecoverableReviewSessionMutationError.value()) };
        }
        return {};
    });
}

[[nodiscard]] std::expected<QString, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::EditReviewSessionToDefault(const QString& CurrentReviewSessionId, const QString& RootDeckId, const QString& ReviewSessionDefinitionKey) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<QString, ReviewSessionMutationErrorEnum> {
        std::expected<QString, Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum> StoreReviewSessionId{
            m_ReviewSessionStore.EditReviewSessionToDefault(CurrentReviewSessionId, RootDeckId, ReviewSessionDefinitionKey)
        };
        if (not StoreReviewSessionId.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(StoreReviewSessionId.error()) };
        }
        return StoreReviewSessionId.value();
    });
}

[[nodiscard]] std::expected<QString, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::EditReviewSessionToCustom(const QString& CurrentReviewSessionId,
                                                const QString& ReviewSessionDefinitionKey,
                                                const QVector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionQVector) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<QString, ReviewSessionMutationErrorEnum> {
        const QVector<Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelection> StoreReviewSessionDeckSelectionQVector{
            ToStoreReviewSessionDeckSelectionQVector(ReviewSessionDeckSelectionQVector)
        };
        std::expected<QString, Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum> StoreReviewSessionId{
            m_ReviewSessionStore.EditReviewSessionToCustom(CurrentReviewSessionId, ReviewSessionDefinitionKey, StoreReviewSessionDeckSelectionQVector)
        };
        if (not StoreReviewSessionId.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(StoreReviewSessionId.error()) };
        }
        return StoreReviewSessionId.value();
    });
}

[[nodiscard]] std::expected<void, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const QString& ReviewSessionId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, ReviewSessionMutationErrorEnum> {
        m_ReviewSessionStore.UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(ReviewSessionId);
        return {};
    });
}

[[nodiscard]] std::expected<void, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::DeleteReviewSession(const QString& ReviewSessionId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, ReviewSessionMutationErrorEnum> {
        m_ReviewSessionStore.DeleteReviewSession(ReviewSessionId);
        return {};
    });
}

}
