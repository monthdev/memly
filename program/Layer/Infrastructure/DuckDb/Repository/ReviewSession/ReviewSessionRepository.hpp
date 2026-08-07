// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Layer/Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Layer/Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Layer/Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Layer/Infrastructure/DuckDb/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/DuckDb/Database/PreparedStatement.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Repository::ReviewSession {

class ReviewSessionRepository final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
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

    [[nodiscard]] auto ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow>;
    [[nodiscard]] auto CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto CreateOrReadExistingCustomReviewSession(const std::string&,
                                                               const std::string&,
                                                               const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto RenameReviewSession(const std::string&, const std::string&)
        -> std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    void UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&);
    void DeleteReviewSession(const std::string&);

private:
    [[nodiscard]] auto TryReadDefaultReviewSessionIdByRootDeckId(const std::string&) -> std::optional<std::string>;
    [[nodiscard]] auto TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string&) -> std::optional<std::string>;
    void CreateCustomReviewSessionDeckSelection(const std::string&,
                                                const std::string&,
                                                Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum);
    void DeleteCustomReviewSessionDeckSelections(const std::string&);
};

}
#endif
