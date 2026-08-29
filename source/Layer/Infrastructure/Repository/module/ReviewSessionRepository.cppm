export module Memly.Repository.ReviewSessionRepository;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <expected>
#include <optional>
#include <string>
#include <vector>

import Memly.Database.DatabaseRuntime;
import Memly.Database.PreparedStatement;
import Memly.Domain.RecoverableReviewSessionMutationError;
import Memly.Domain.ReviewSessionDeckSelection;
import Memly.Domain.ReviewSessionListRow;

export namespace Memly::Repository {

class ReviewSessionRepository final {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
    Database::PreparedStatement m_SelectReviewSessionListRowsPreparedStatement;
    Database::PreparedStatement
        m_SelectDefaultReviewSessionIdByRootDeckIdPreparedStatement;
    Database::PreparedStatement
        m_SelectReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement;
    Database::PreparedStatement m_InsertCustomReviewSessionPreparedStatement;
    Database::PreparedStatement m_InsertDefaultReviewSessionPreparedStatement;
    Database::PreparedStatement
        m_InsertCustomReviewSessionDeckSelectionPreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionNamePreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionToDefaultPreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionToCustomPreparedStatement;
    Database::PreparedStatement
        m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement;
    Database::PreparedStatement
        m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement;
    Database::PreparedStatement m_DeleteReviewSessionPreparedStatement;

public:
    explicit ReviewSessionRepository(Database::DatabaseRuntime&);

    explicit ReviewSessionRepository(const ReviewSessionRepository&) = delete;
    ReviewSessionRepository&
    operator=(const ReviewSessionRepository&) = delete;

    explicit ReviewSessionRepository(ReviewSessionRepository&&) = delete;
    ReviewSessionRepository&
    operator=(ReviewSessionRepository&&) = delete;

    ~ReviewSessionRepository() noexcept = default;

    [[nodiscard]] std::vector<Domain::ReviewSessionListRow>
    ReadReviewSessionListRows();

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

    [[nodiscard]] std::optional<
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

    void
    UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(
        const std::string&
    );

    void
    DeleteReviewSession(const std::string&);

private:
    [[nodiscard]] std::optional<std::string>
    TryReadDefaultReviewSessionIdByRootDeckId(const std::string&);

    [[nodiscard]] std::optional<std::string>
    TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string&);

    void
    CreateCustomReviewSessionDeckSelection(
        const std::string&,
        const std::string&,
        Domain::ReviewSessionDeckSelection::DeckSelectionTypeEnum
    );

    void
    DeleteCustomReviewSessionDeckSelections(const std::string&);
};

}
#endif
