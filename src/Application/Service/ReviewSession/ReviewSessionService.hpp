#pragma once

#include <expected>
#include <string>
#include <vector>

#include "Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Domain/ReviewSession/ReviewSessionDeckSelection.hpp"

namespace Infrastructure::Sql {
class TransactionRunner;
}

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionService final {
public:
    ReviewSessionService(Infrastructure::Sql::TransactionRunner& TransactionRunner,
                         Infrastructure::Store::ReviewSession::ReviewSessionStore& ReviewSessionStore) noexcept
        : m_TransactionRunner{ TransactionRunner }
        , m_ReviewSessionStore{ ReviewSessionStore } {
    }

    ~ReviewSessionService() noexcept = default;
    ReviewSessionService(const ReviewSessionService&) = delete;
    ReviewSessionService(ReviewSessionService&&) = delete;
    ReviewSessionService& operator=(const ReviewSessionService&) = delete;
    ReviewSessionService& operator=(ReviewSessionService&&) = delete;

    [[nodiscard]] std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&);
    [[nodiscard]] std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    CreateOrReadExistingCustomReviewSession(const std::string&, const std::string&, const std::vector<Domain::ReviewSession::ReviewSessionDeckSelection>&);
    [[nodiscard]] std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RenameReviewSession(const std::string&,
                                                                                                                            const std::string&);
    [[nodiscard]] std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&);
    [[nodiscard]] std::expected<std::string, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<Domain::ReviewSession::ReviewSessionDeckSelection>&);
    [[nodiscard]] std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&);
    [[nodiscard]] std::expected<void, Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> DeleteReviewSession(const std::string&);

private:
    Infrastructure::Sql::TransactionRunner& m_TransactionRunner;
    Infrastructure::Store::ReviewSession::ReviewSessionStore& m_ReviewSessionStore;
};

}
