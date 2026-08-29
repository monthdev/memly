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
    Repository::ReviewSessionRepository& ReviewSessionRepository
) noexcept
    : m_TransactionRunner{ TransactionRunner }
    , m_ReviewSessionRepository{ ReviewSessionRepository } {
}

[[nodiscard]] std::expected<
    std::string,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::CreateOrReadExistingDefaultReviewSession(
    const std::string& RootDeckId,
    const std::string& ReviewSessionDefinitionKey
) {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<
                  std::string,
                  Domain::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionRepository
                .CreateOrReadExistingDefaultReviewSession(
                    RootDeckId,
                    ReviewSessionDefinitionKey
                );
        }
    );
}

[[nodiscard]] std::expected<
    std::string,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSessionDeckSelection>&
        ReviewSessionDeckSelectionVector
) {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<
                  std::string,
                  Domain::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionRepository
                .CreateOrReadExistingCustomReviewSession(
                    ReviewSessionName,
                    ReviewSessionDefinitionKey,
                    ReviewSessionDeckSelectionVector
                );
        }
    );
}

[[nodiscard]] std::expected<
    void,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::RenameReviewSession(
    const std::string& ReviewSessionId,
    const std::string& ReviewSessionName
) {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<
                  void,
                  Domain::RecoverableReviewSessionMutationErrorEnum> {
            const std::optional<
                Domain::RecoverableReviewSessionMutationErrorEnum>
                RecoverableReviewSessionMutationErrorOptional{
                    this->m_ReviewSessionRepository
                        .RenameReviewSession(ReviewSessionId, ReviewSessionName)
                };
            if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
                return std::unexpected{
                    RecoverableReviewSessionMutationErrorOptional.value()
                };
            }
            return std::expected<
                void,
                Domain::RecoverableReviewSessionMutationErrorEnum>{};
        }
    );
}

[[nodiscard]] std::expected<
    std::string,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::EditReviewSessionToDefault(
    const std::string& CurrentReviewSessionId,
    const std::string& RootDeckId,
    const std::string& ReviewSessionDefinitionKey
) {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<
                  std::string,
                  Domain::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionRepository.EditReviewSessionToDefault(
                CurrentReviewSessionId,
                RootDeckId,
                ReviewSessionDefinitionKey
            );
        }
    );
}

[[nodiscard]] std::expected<
    std::string,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::EditReviewSessionToCustom(
    const std::string& CurrentReviewSessionId,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSessionDeckSelection>&
        ReviewSessionDeckSelectionVector
) {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<
                  std::string,
                  Domain::RecoverableReviewSessionMutationErrorEnum> {
            return this->m_ReviewSessionRepository.EditReviewSessionToCustom(
                CurrentReviewSessionId,
                ReviewSessionDefinitionKey,
                ReviewSessionDeckSelectionVector
            );
        }
    );
}

[[nodiscard]] std::expected<
    void,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(
    const std::string& ReviewSessionId
) {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<
                  void,
                  Domain::RecoverableReviewSessionMutationErrorEnum> {
            this->m_ReviewSessionRepository
                .UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(
                    ReviewSessionId
                );
            return std::expected<
                void,
                Domain::RecoverableReviewSessionMutationErrorEnum>{};
        }
    );
}

[[nodiscard]] std::expected<
    void,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionService::DeleteReviewSession(const std::string& ReviewSessionId) {
    return this->m_TransactionRunner.TransactionWrapper(
        [&]() -> std::expected<
                  void,
                  Domain::RecoverableReviewSessionMutationErrorEnum> {
            this->m_ReviewSessionRepository
                .DeleteReviewSession(ReviewSessionId);
            return std::expected<
                void,
                Domain::RecoverableReviewSessionMutationErrorEnum>{};
        }
    );
}

}
#endif
