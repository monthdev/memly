#pragma once

#include <expected>
#include <string>
#include <vector>

#include "Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"

namespace Infrastructure::Database {
class TransactionRunner;
}

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionService final {
private:
    Infrastructure::Database::TransactionRunner& m_TransactionRunner;
    Infrastructure::Store::ReviewSession::ReviewSessionStore& m_ReviewSessionStore;

public:
    explicit ReviewSessionService(Infrastructure::Database::TransactionRunner& TransactionRunner,
                                  Infrastructure::Store::ReviewSession::ReviewSessionStore& ReviewSessionStore) noexcept
        : m_TransactionRunner{ TransactionRunner }
        , m_ReviewSessionStore{ ReviewSessionStore } {
    }

    explicit ReviewSessionService(const ReviewSessionService&) = delete;
    explicit ReviewSessionService(ReviewSessionService&&) = delete;
    auto operator=(const ReviewSessionService&) -> ReviewSessionService& = delete;
    auto operator=(ReviewSessionService&&) -> ReviewSessionService& = delete;

    [[nodiscard]] auto CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto CreateOrReadExistingCustomReviewSession(const std::string&,
                                                               const std::string&,
                                                               const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto RenameReviewSession(const std::string&, const std::string&)
        -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&)
        -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto DeleteReviewSession(const std::string&)
        -> std::expected<void, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
};

}
