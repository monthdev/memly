#pragma once

#include <duckdb.hpp>

#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/ReviewSession/Mutation/ReviewSessionMutationSql.hpp"
#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionStore final {
public:
    explicit ReviewSessionStore(duckdb::Connection& DatabaseConnection)
        : m_CreateCustomReviewSessionPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Mutation::CreateCustomReviewSessionSql()) }
        , m_CreateDefaultReviewSessionPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Mutation::CreateDefaultReviewSessionSql()) }
        , m_CreateCustomReviewSessionDeckSelectionPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Mutation::CreateCustomReviewSessionDeckSelectionSql()) }
        , m_RenameReviewSessionPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::ReviewSession::Mutation::RenameReviewSessionSql()) }
        , m_UpdateReviewSessionToDefaultPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Mutation::UpdateReviewSessionToDefaultSql()) }
        , m_UpdateReviewSessionToCustomPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Mutation::UpdateReviewSessionToCustomSql()) }
        , m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Mutation::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql()) }
        , m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Mutation::DeleteCustomReviewSessionDeckSelectionsSql()) }
        , m_DeleteReviewSessionPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::ReviewSession::Mutation::DeleteReviewSessionSql()) }
        , m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Query::ReadDefaultReviewSessionIdByRootDeckIdSql()) }
        , m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::ReviewSession::Query::ReadReviewSessionIdByReviewSessionDefinitionKeySql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_CreateCustomReviewSessionPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_CreateDefaultReviewSessionPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_CreateCustomReviewSessionDeckSelectionPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_RenameReviewSessionPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_UpdateReviewSessionToDefaultPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_UpdateReviewSessionToCustomPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_DeleteReviewSessionPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement);
    }

    ReviewSessionStore(const ReviewSessionStore&) = delete;
    ReviewSessionStore(ReviewSessionStore&&) = delete;
    ReviewSessionStore& operator=(const ReviewSessionStore&) = delete;
    ReviewSessionStore& operator=(ReviewSessionStore&&) = delete;

    [[nodiscard]] std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&);
    [[nodiscard]] std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    CreateOrReadExistingCustomReviewSession(const std::string&,
                                            const std::string&,
                                            const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>&);
    [[nodiscard]] std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RenameReviewSession(const std::string&,
                                                                                                                                   const std::string&);
    [[nodiscard]] std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&);
    [[nodiscard]] std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>&);
    void UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&);
    void DeleteReviewSession(const std::string&);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_CreateCustomReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateDefaultReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateCustomReviewSessionDeckSelectionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_RenameReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_UpdateReviewSessionToDefaultPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_UpdateReviewSessionToCustomPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement;

    [[nodiscard]] std::optional<std::string> TryReadDefaultReviewSessionIdByRootDeckId(const std::string&);
    [[nodiscard]] std::optional<std::string> TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string&);
    [[nodiscard]] std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>
    CreateCustomReviewSessionDeckSelection(const std::string&,
                                           const std::string&,
                                           Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum);
    void DeleteCustomReviewSessionDeckSelections(const std::string&);
};

}
