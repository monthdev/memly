#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace Infrastructure::Sql {
class TransactionRunner;
}

namespace Infrastructure::Store::ReviewSession {
class ReviewSessionStore;
}

namespace Application::Service::ReviewSession {

class ReviewSessionService final {
public:
    enum class ReviewSessionMutationErrorEnum : std::uint8_t {
        ReviewSessionNameLengthError,
        DuplicateReviewSessionDefinitionKeyError,
        ConflictingReviewSessionDeckSelfSelectionError,
        ConflictingReviewSessionDeckSubtreeSelectionError,
        ConflictingReviewSessionDeckIncludeSelectionError,
        ConflictingReviewSessionDeckExcludeSelectionError
    };

    enum class ReviewSessionDeckSelectionTypeEnum : std::uint8_t {
        Self,
        Subtree,
        ExcludeSelf,
        ExcludeSubtree
    };

    struct ReviewSessionDeckSelection {
        std::string m_DeckId;
        ReviewSessionDeckSelectionTypeEnum m_DeckSelectionType;
    };

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

    [[nodiscard]] std::expected<std::string, ReviewSessionMutationErrorEnum> CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&);
    [[nodiscard]] std::expected<std::string, ReviewSessionMutationErrorEnum>
    CreateOrReadExistingCustomReviewSession(const std::string&, const std::string&, const std::vector<ReviewSessionDeckSelection>&);
    [[nodiscard]] std::expected<void, ReviewSessionMutationErrorEnum> RenameReviewSession(const std::string&, const std::string&);
    [[nodiscard]] std::expected<std::string, ReviewSessionMutationErrorEnum>
    EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&);
    [[nodiscard]] std::expected<std::string, ReviewSessionMutationErrorEnum>
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<ReviewSessionDeckSelection>&);
    [[nodiscard]] std::expected<void, ReviewSessionMutationErrorEnum> UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&);
    [[nodiscard]] std::expected<void, ReviewSessionMutationErrorEnum> DeleteReviewSession(const std::string&);

private:
    Infrastructure::Sql::TransactionRunner& m_TransactionRunner;
    Infrastructure::Store::ReviewSession::ReviewSessionStore& m_ReviewSessionStore;
};

}
