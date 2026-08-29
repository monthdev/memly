export module Memly.Service.ReviewSessionService;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <expected>
#include <string>
#include <vector>

import Memly.Database.TransactionRunner;
import Memly.Domain.RecoverableReviewSessionMutationError;
import Memly.Domain.ReviewSessionDeckSelection;
import Memly.Repository.ReviewSessionRepository;

export namespace Memly::Service {

class ReviewSessionService final {
private:
    Database::TransactionRunner& m_TransactionRunner;
    Repository::ReviewSessionRepository& m_ReviewSessionRepository;

public:
    explicit ReviewSessionService(
        Database::TransactionRunner&,
        Repository::ReviewSessionRepository&
    ) noexcept;

    explicit ReviewSessionService(const ReviewSessionService&) = delete;
    ReviewSessionService&
    operator=(const ReviewSessionService&) = delete;

    explicit ReviewSessionService(ReviewSessionService&&) = delete;
    ReviewSessionService&
    operator=(ReviewSessionService&&) = delete;

    ~ReviewSessionService() noexcept = default;

    [[nodiscard]] std::expected<
        std::string,
        Domain::RecoverableReviewSessionMutationErrorEnum>
    CreateOrReadExistingDefaultReviewSession(
        const std::string&,
        const std::string&
    );

    [[nodiscard]] std::expected<
        std::string,
        Domain::RecoverableReviewSessionMutationErrorEnum>
    CreateOrReadExistingCustomReviewSession(
        const std::string&,
        const std::string&,
        const std::vector<Domain::ReviewSessionDeckSelection>&
    );

    [[nodiscard]] std::expected<
        void,
        Domain::RecoverableReviewSessionMutationErrorEnum>
    RenameReviewSession(const std::string&, const std::string&);

    [[nodiscard]] std::expected<
        std::string,
        Domain::RecoverableReviewSessionMutationErrorEnum>
    EditReviewSessionToDefault(
        const std::string&,
        const std::string&,
        const std::string&
    );

    [[nodiscard]] std::expected<
        std::string,
        Domain::RecoverableReviewSessionMutationErrorEnum>
    EditReviewSessionToCustom(
        const std::string&,
        const std::string&,
        const std::vector<Domain::ReviewSessionDeckSelection>&
    );

    [[nodiscard]] std::expected<
        void,
        Domain::RecoverableReviewSessionMutationErrorEnum>
    UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(
        const std::string&
    );

    [[nodiscard]] std::expected<
        void,
        Domain::RecoverableReviewSessionMutationErrorEnum>
    DeleteReviewSession(const std::string&);
};

}
#endif
