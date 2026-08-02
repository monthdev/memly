// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Application/Service/ReviewSession/ReviewSessionService.hpp"

#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Layer/Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Layer/Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Layer/Infrastructure/Database/TransactionRunner.hpp"
#include "Layer/Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

namespace Layer::Application::Service::ReviewSession {

[[nodiscard]] auto ReviewSessionService::CreateOrReadExistingDefaultReviewSession(const std::string& RootDeckId, const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionStore.CreateOrReadExistingDefaultReviewSession(RootDeckId, ReviewSessionDefinitionKey);
        });
}

[[nodiscard]] auto ReviewSessionService::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionStore.CreateOrReadExistingCustomReviewSession(
                ReviewSessionName, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionVector);
        });
}

[[nodiscard]] auto ReviewSessionService::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName)
    -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        const std::optional<Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
            this->m_ReviewSessionStore.RenameReviewSession(ReviewSessionId, ReviewSessionName)
        };
        if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
        }
        return std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>{};
    });
}

[[nodiscard]] auto ReviewSessionService::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                                                    const std::string& RootDeckId,
                                                                    const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionStore.EditReviewSessionToDefault(CurrentReviewSessionId, RootDeckId, ReviewSessionDefinitionKey);
        });
}

[[nodiscard]] auto ReviewSessionService::EditReviewSessionToCustom(
    const std::string& CurrentReviewSessionId,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionStore.EditReviewSessionToCustom(CurrentReviewSessionId, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionVector);
        });
}

[[nodiscard]] auto ReviewSessionService::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string& ReviewSessionId)
    -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        this->m_ReviewSessionStore.UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(ReviewSessionId);
        return std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>{};
    });
}

[[nodiscard]] auto ReviewSessionService::DeleteReviewSession(const std::string& ReviewSessionId)
    -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
        this->m_ReviewSessionStore.DeleteReviewSession(ReviewSessionId);
        return std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>{};
    });
}

}
#endif
