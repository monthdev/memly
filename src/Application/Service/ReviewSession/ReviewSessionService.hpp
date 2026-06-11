#pragma once

#include <QString>
#include <QVector>
#include <cstdint>
#include <expected>

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
        QString m_DeckId;
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

    [[nodiscard]] std::expected<QString, ReviewSessionMutationErrorEnum> CreateOrReadExistingDefaultReviewSession(const QString&, const QString&);
    [[nodiscard]] std::expected<QString, ReviewSessionMutationErrorEnum>
    CreateOrReadExistingCustomReviewSession(const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    [[nodiscard]] std::expected<void, ReviewSessionMutationErrorEnum> RenameReviewSession(const QString&, const QString&);
    [[nodiscard]] std::expected<QString, ReviewSessionMutationErrorEnum> EditReviewSessionToDefault(const QString&, const QString&, const QString&);
    [[nodiscard]] std::expected<QString, ReviewSessionMutationErrorEnum>
    EditReviewSessionToCustom(const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    [[nodiscard]] std::expected<void, ReviewSessionMutationErrorEnum> UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const QString&);
    [[nodiscard]] std::expected<void, ReviewSessionMutationErrorEnum> DeleteReviewSession(const QString&);

private:
    Infrastructure::Sql::TransactionRunner& m_TransactionRunner;
    Infrastructure::Store::ReviewSession::ReviewSessionStore& m_ReviewSessionStore;
};

}
