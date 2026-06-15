#include "Application/Service/ReviewSession/ReviewSessionService.hpp"

#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Domain/ReviewSession/ReviewSessionDeckSelectionData.hpp"
#include "Infrastructure/Sql/TransactionRunner.hpp"
#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

namespace Application::Service::ReviewSession {

[[nodiscard]] std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::CreateOrReadExistingDefaultReviewSession(const std::string& RootDeckId, const std::string& ReviewSessionDefinitionKey) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        return m_ReviewSessionStore.CreateOrReadExistingDefaultReviewSession(RootDeckId, ReviewSessionDefinitionKey);
    });
}

[[nodiscard]] std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSession::ReviewSessionDeckSelectionData>& ReviewSessionDeckSelectionVector) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        return m_ReviewSessionStore.CreateOrReadExistingCustomReviewSession(ReviewSessionName, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionVector);
    });
}

[[nodiscard]] std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        const std::optional<Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{
            m_ReviewSessionStore.RenameReviewSession(ReviewSessionId, ReviewSessionName)
        };
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationError.value() };
        }
        return {};
    });
}

[[nodiscard]] std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                                 const std::string& RootDeckId,
                                                 const std::string& ReviewSessionDefinitionKey) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        return m_ReviewSessionStore.EditReviewSessionToDefault(CurrentReviewSessionId, RootDeckId, ReviewSessionDefinitionKey);
    });
}

[[nodiscard]] std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::EditReviewSessionToCustom(const std::string& CurrentReviewSessionId,
                                                const std::string& ReviewSessionDefinitionKey,
                                                const std::vector<Domain::ReviewSession::ReviewSessionDeckSelectionData>& ReviewSessionDeckSelectionVector) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        return m_ReviewSessionStore.EditReviewSessionToCustom(CurrentReviewSessionId, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionVector);
    });
}

[[nodiscard]] std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string& ReviewSessionId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        m_ReviewSessionStore.UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(ReviewSessionId);
        return {};
    });
}

[[nodiscard]] std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::DeleteReviewSession(const std::string& ReviewSessionId) {
    return m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        m_ReviewSessionStore.DeleteReviewSession(ReviewSessionId);
        return {};
    });
}

}
