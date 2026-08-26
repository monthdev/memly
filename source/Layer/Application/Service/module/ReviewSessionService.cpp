module Memly.Service.ReviewSessionService;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Service/ReviewSessionService.hpp"

#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Memly/Database/TransactionRunner.hpp"
#include "Memly/Domain/RecoverableReviewSessionMutationError.hpp"
#include "Memly/Domain/ReviewSessionDeckSelection.hpp"
#include "Memly/Repository/ReviewSessionRepository.hpp"

namespace Memly::Service {

ReviewSessionService::ReviewSessionService(
    Database::TransactionRunner& TransactionRunner,
    Repository::ReviewSessionRepository& ReviewSessionRepository) noexcept
    : m_TransactionRunner{ TransactionRunner }
    , m_ReviewSessionRepository{ ReviewSessionRepository } {
}

[[nodiscard]] auto ReviewSessionService::CreateOrReadExistingDefaultReviewSession(const std::string& RootDeckId, const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionRepository.CreateOrReadExistingDefaultReviewSession(RootDeckId, ReviewSessionDefinitionKey);
        });
}

[[nodiscard]] auto ReviewSessionService::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionRepository.CreateOrReadExistingCustomReviewSession(
                ReviewSessionName, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionVector);
        });
}

[[nodiscard]] auto ReviewSessionService::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName)
    -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum> {
        const std::optional<Domain::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
            this->m_ReviewSessionRepository.RenameReviewSession(ReviewSessionId, ReviewSessionName)
        };
        if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
        }
        return std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum>{};
    });
}

[[nodiscard]] auto ReviewSessionService::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                                                    const std::string& RootDeckId,
                                                                    const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionRepository.EditReviewSessionToDefault(CurrentReviewSessionId, RootDeckId, ReviewSessionDefinitionKey);
        });
}

[[nodiscard]] auto ReviewSessionService::EditReviewSessionToCustom(
    const std::string& CurrentReviewSessionId,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionRepository.EditReviewSessionToCustom(CurrentReviewSessionId, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionVector);
        });
}

[[nodiscard]] auto ReviewSessionService::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string& ReviewSessionId)
    -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum> {
        this->m_ReviewSessionRepository.UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(ReviewSessionId);
        return std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum>{};
    });
}

[[nodiscard]] auto ReviewSessionService::DeleteReviewSession(const std::string& ReviewSessionId)
    -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum> {
    return this->m_TransactionRunner.TransactionWrapper([&]() -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum> {
        this->m_ReviewSessionRepository.DeleteReviewSession(ReviewSessionId);
        return std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum>{};
    });
}

}
#endif
