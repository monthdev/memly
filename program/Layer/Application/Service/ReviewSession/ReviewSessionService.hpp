// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <expected>
#include <string>
#include <vector>

#include "Layer/Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Layer/Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Persistence::Database {
class TransactionRunner;
}

namespace Layer::Infrastructure::Persistence::Repository::ReviewSession {
class ReviewSessionRepository;
}

namespace Layer::Application::Service::ReviewSession {

class ReviewSessionService final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::Persistence::Database::TransactionRunner& m_TransactionRunner;
    Infrastructure::Persistence::Repository::ReviewSession::ReviewSessionRepository& m_ReviewSessionRepository;

public:
    explicit ReviewSessionService(Infrastructure::Persistence::Database::TransactionRunner& TransactionRunner,
                                  Infrastructure::Persistence::Repository::ReviewSession::ReviewSessionRepository& ReviewSessionRepository) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_TransactionRunner{ TransactionRunner }
        , m_ReviewSessionRepository{ ReviewSessionRepository } {
    }

    [[nodiscard]] auto CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&)
        -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto CreateOrReadExistingCustomReviewSession(const std::string&,
                                                               const std::string&,
                                                               const std::vector<Domain::ReviewSession::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto RenameReviewSession(const std::string&, const std::string&)
        -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&)
        -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<Domain::ReviewSession::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&)
        -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto DeleteReviewSession(const std::string&)
        -> std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
};

}
#endif
