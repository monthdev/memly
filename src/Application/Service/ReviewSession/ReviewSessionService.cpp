#include "Application/Service/ReviewSession/ReviewSessionService.hpp"

#include <optional>
#include <string>
#include <vector>

#include "Infrastructure/Sql/TransactionRunner.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

namespace Application::Service::ReviewSession {
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

[[nodiscard]] std::vector<Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelection> ToStoreReviewSessionDeckSelectionVector(
    const std::vector<Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector) {
    std::vector<Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelection> StoreReviewSessionDeckSelectionVector{};
    StoreReviewSessionDeckSelectionVector.reserve(ReviewSessionDeckSelectionVector.size());
    for (const Application::Service::ReviewSession::ReviewSessionService::ReviewSessionDeckSelection& ReviewSessionDeckSelection :
         ReviewSessionDeckSelectionVector) {
        StoreReviewSessionDeckSelectionVector.emplace_back(ReviewSessionDeckSelection.m_DeckId,
                                                           ToStoreReviewSessionDeckSelectionType(ReviewSessionDeckSelection.m_DeckSelectionType));
    }
    return StoreReviewSessionDeckSelectionVector;
}

}

[[nodiscard]] std::expected<std::string, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::CreateOrReadExistingDefaultReviewSession(const std::string& RootDeckId, const std::string& ReviewSessionDefinitionKey) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<std::string, ReviewSessionMutationErrorEnum> {
        std::expected<std::string, Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum> StoreReviewSessionId{
            m_ReviewSessionStore.CreateOrReadExistingDefaultReviewSession(RootDeckId, ReviewSessionDefinitionKey)
        };
        if (not StoreReviewSessionId.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(StoreReviewSessionId.error()) };
        }
        return StoreReviewSessionId.value();
    });
}

[[nodiscard]] std::expected<std::string, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::CreateOrReadExistingCustomReviewSession(const std::string& ReviewSessionName,
                                                              const std::string& ReviewSessionDefinitionKey,
                                                              const std::vector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<std::string, ReviewSessionMutationErrorEnum> {
        const std::vector<Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelection> StoreReviewSessionDeckSelectionVector{
            ToStoreReviewSessionDeckSelectionVector(ReviewSessionDeckSelectionVector)
        };
        std::expected<std::string, Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum> StoreReviewSessionId{
            m_ReviewSessionStore.CreateOrReadExistingCustomReviewSession(ReviewSessionName, ReviewSessionDefinitionKey, StoreReviewSessionDeckSelectionVector)
        };
        if (not StoreReviewSessionId.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(StoreReviewSessionId.error()) };
        }
        return StoreReviewSessionId.value();
    });
}

[[nodiscard]] std::expected<void, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, ReviewSessionMutationErrorEnum> {
        const std::optional<Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
            RecoverableReviewSessionMutationError{ m_ReviewSessionStore.RenameReviewSession(ReviewSessionId, ReviewSessionName) };
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(RecoverableReviewSessionMutationError.value()) };
        }
        return {};
    });
}

[[nodiscard]] std::expected<std::string, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                                 const std::string& RootDeckId,
                                                 const std::string& ReviewSessionDefinitionKey) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<std::string, ReviewSessionMutationErrorEnum> {
        std::expected<std::string, Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum> StoreReviewSessionId{
            m_ReviewSessionStore.EditReviewSessionToDefault(CurrentReviewSessionId, RootDeckId, ReviewSessionDefinitionKey)
        };
        if (not StoreReviewSessionId.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(StoreReviewSessionId.error()) };
        }
        return StoreReviewSessionId.value();
    });
}

[[nodiscard]] std::expected<std::string, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::EditReviewSessionToCustom(const std::string& CurrentReviewSessionId,
                                                const std::string& ReviewSessionDefinitionKey,
                                                const std::vector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<std::string, ReviewSessionMutationErrorEnum> {
        const std::vector<Infrastructure::Store::ReviewSession::ReviewSessionStore::ReviewSessionDeckSelection> StoreReviewSessionDeckSelectionVector{
            ToStoreReviewSessionDeckSelectionVector(ReviewSessionDeckSelectionVector)
        };
        std::expected<std::string, Infrastructure::Store::ReviewSession::ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum> StoreReviewSessionId{
            m_ReviewSessionStore.EditReviewSessionToCustom(CurrentReviewSessionId, ReviewSessionDefinitionKey, StoreReviewSessionDeckSelectionVector)
        };
        if (not StoreReviewSessionId.has_value()) {
            return std::unexpected{ ToReviewSessionMutationError(StoreReviewSessionId.error()) };
        }
        return StoreReviewSessionId.value();
    });
}

[[nodiscard]] std::expected<void, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string& ReviewSessionId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, ReviewSessionMutationErrorEnum> {
        m_ReviewSessionStore.UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(ReviewSessionId);
        return {};
    });
}

[[nodiscard]] std::expected<void, ReviewSessionService::ReviewSessionMutationErrorEnum>
ReviewSessionService::DeleteReviewSession(const std::string& ReviewSessionId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, ReviewSessionMutationErrorEnum> {
        m_ReviewSessionStore.DeleteReviewSession(ReviewSessionId);
        return {};
    });
}

}
