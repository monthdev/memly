#include "Application/Service/ReviewSession/ReviewSessionService.hpp"

#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Infrastructure/Database/TransactionRunner.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

namespace Application::Service::ReviewSession {

[[nodiscard]] auto ReviewSessionService::CreateOrReadExistingDefaultReviewSession(const std::string& RootDeckId, const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
            return m_ReviewSessionStore.CreateOrReadExistingDefaultReviewSession(RootDeckId, ReviewSessionDefinitionKey);
        });
}

[[nodiscard]] auto ReviewSessionService::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
            return m_ReviewSessionStore.CreateOrReadExistingCustomReviewSession(
                ReviewSessionName, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionVector);
        });
}

[[nodiscard]] auto ReviewSessionService::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName)
    -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        const std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{
            m_ReviewSessionStore.RenameReviewSession(ReviewSessionId, ReviewSessionName)
        };
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationError.value() };
        }
        return {};
    });
}

[[nodiscard]] auto ReviewSessionService::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                                                    const std::string& RootDeckId,
                                                                    const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
            return m_ReviewSessionStore.EditReviewSessionToDefault(CurrentReviewSessionId, RootDeckId, ReviewSessionDefinitionKey);
        });
}

[[nodiscard]] auto ReviewSessionService::EditReviewSessionToCustom(
    const std::string& CurrentReviewSessionId,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
            return m_ReviewSessionStore.EditReviewSessionToCustom(CurrentReviewSessionId, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionVector);
        });
}

[[nodiscard]] auto ReviewSessionService::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string& ReviewSessionId)
    -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        m_ReviewSessionStore.UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(ReviewSessionId);
        return {};
    });
}

[[nodiscard]] auto ReviewSessionService::DeleteReviewSession(const std::string& ReviewSessionId)
    -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        m_ReviewSessionStore.DeleteReviewSession(ReviewSessionId);
        return {};
    });
}

}
