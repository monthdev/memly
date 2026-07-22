#pragma once

#include <expected>
#include <string>
#include <vector>

#include "Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Database {
class TransactionRunner;
}

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionService final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::Database::TransactionRunner& m_TransactionRunner;
    Infrastructure::Store::ReviewSession::ReviewSessionStore& m_ReviewSessionStore;

public:
    explicit ReviewSessionService(Infrastructure::Database::TransactionRunner& TransactionRunner,
                                  Infrastructure::Store::ReviewSession::ReviewSessionStore& ReviewSessionStore) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_TransactionRunner{ TransactionRunner }
        , m_ReviewSessionStore{ ReviewSessionStore } {
    }

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
