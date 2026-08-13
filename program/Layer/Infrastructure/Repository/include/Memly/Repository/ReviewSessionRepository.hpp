// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Memly/Database/DatabaseRuntime.hpp"
#include "Memly/Database/PreparedStatement.hpp"
#include "Memly/Domain/RecoverableReviewSessionMutationError.hpp"
#include "Memly/Domain/ReviewSessionDeckSelection.hpp"
#include "Memly/Domain/ReviewSessionListRow.hpp"

namespace Layer::Infrastructure::Repository {

class ReviewSessionRepository final {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
    Database::PreparedStatement m_SelectReviewSessionListRowsPreparedStatement;
    Database::PreparedStatement m_SelectDefaultReviewSessionIdByRootDeckIdPreparedStatement;
    Database::PreparedStatement m_SelectReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement;
    Database::PreparedStatement m_InsertCustomReviewSessionPreparedStatement;
    Database::PreparedStatement m_InsertDefaultReviewSessionPreparedStatement;
    Database::PreparedStatement m_InsertCustomReviewSessionDeckSelectionPreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionNamePreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionToDefaultPreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionToCustomPreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement;
    Database::PreparedStatement m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement;
    Database::PreparedStatement m_DeleteReviewSessionPreparedStatement;

public:
    explicit ReviewSessionRepository(Database::DatabaseRuntime&);

    explicit ReviewSessionRepository(const ReviewSessionRepository&) = delete;
    auto operator=(const ReviewSessionRepository&) -> ReviewSessionRepository& = delete;

    explicit ReviewSessionRepository(ReviewSessionRepository&&) = delete;
    auto operator=(ReviewSessionRepository&&) -> ReviewSessionRepository& = delete;

    ~ReviewSessionRepository() noexcept = default;

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSessionListRow>;
    [[nodiscard]] auto CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&)
        -> std::expected<std::string, Application::Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto CreateOrReadExistingCustomReviewSession(const std::string&,
                                                               const std::string&,
                                                               const std::vector<Application::Domain::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Application::Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto RenameReviewSession(const std::string&, const std::string&)
        -> std::optional<Application::Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&)
        -> std::expected<std::string, Application::Domain::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<Application::Domain::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Application::Domain::RecoverableReviewSessionMutationErrorEnum>;
    void UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&);
    void DeleteReviewSession(const std::string&);

private:
    [[nodiscard]] auto TryReadDefaultReviewSessionIdByRootDeckId(const std::string&) -> std::optional<std::string>;
    [[nodiscard]] auto TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string&) -> std::optional<std::string>;
    void CreateCustomReviewSessionDeckSelection(const std::string&,
                                                const std::string&,
                                                Application::Domain::ReviewSessionDeckSelection::DeckSelectionTypeEnum);
    void DeleteCustomReviewSessionDeckSelections(const std::string&);
};

}
#endif
