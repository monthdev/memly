// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <expected>
#include <string>
#include <vector>

#include "Memly/Database/TransactionRunner.hpp"
#include "Memly/Domain/RecoverableReviewSessionMutationError.hpp"
#include "Memly/Domain/ReviewSessionDeckSelection.hpp"
#include "Memly/Repository/ReviewSessionRepository.hpp"

namespace Memly::Service {

class ReviewSessionService final {
private:
    Database::TransactionRunner& m_TransactionRunner;
    Repository::ReviewSessionRepository& m_ReviewSessionRepository;

public:
    explicit ReviewSessionService(Database::TransactionRunner&,
                                  Repository::ReviewSessionRepository&) noexcept;

    explicit ReviewSessionService(const ReviewSessionService&) = delete;
    auto operator=(const ReviewSessionService&) -> ReviewSessionService& = delete;

    explicit ReviewSessionService(ReviewSessionService&&) = delete;
    auto operator=(ReviewSessionService&&) -> ReviewSessionService& = delete;

    ~ReviewSessionService() noexcept = default;

    [[nodiscard]] auto CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&)
        -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto CreateOrReadExistingCustomReviewSession(const std::string&,
                                                               const std::string&,
                                                               const std::vector<Domain::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto RenameReviewSession(const std::string&, const std::string&)
        -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&)
        -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<Domain::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&)
        -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto DeleteReviewSession(const std::string&)
        -> std::expected<void, Domain::RecoverableReviewSessionMutationErrorEnum>;
};

}
#endif
