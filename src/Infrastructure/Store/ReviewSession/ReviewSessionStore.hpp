#pragma once

#include <duckdb.hpp>

#include <QString>
#include <QVector>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>

#include "Infrastructure/Sql/ReviewSession/Mutation/ReviewSessionMutationSql.hpp"
#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionStore final {
public:
    enum class RecoverableReviewSessionMutationErrorEnum : std::uint8_t {
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
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateCustomReviewSessionPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateDefaultReviewSessionPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateCustomReviewSessionDeckSelectionPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_RenameReviewSessionPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_UpdateReviewSessionToDefaultPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_UpdateReviewSessionToCustomPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteReviewSessionPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement);
    }

    ~ReviewSessionStore() noexcept = default;
    ReviewSessionStore(const ReviewSessionStore&) = delete;
    ReviewSessionStore(ReviewSessionStore&&) = delete;
    ReviewSessionStore& operator=(const ReviewSessionStore&) = delete;
    ReviewSessionStore& operator=(ReviewSessionStore&&) = delete;

    [[nodiscard]] std::expected<QString, RecoverableReviewSessionMutationErrorEnum> CreateOrReadExistingDefaultReviewSession(const QString&, const QString&);
    [[nodiscard]] std::expected<QString, RecoverableReviewSessionMutationErrorEnum>
    CreateOrReadExistingCustomReviewSession(const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    [[nodiscard]] std::optional<RecoverableReviewSessionMutationErrorEnum> RenameReviewSession(const QString&, const QString&);
    [[nodiscard]] std::expected<QString, RecoverableReviewSessionMutationErrorEnum> EditReviewSessionToDefault(const QString&, const QString&, const QString&);
    [[nodiscard]] std::expected<QString, RecoverableReviewSessionMutationErrorEnum>
    EditReviewSessionToCustom(const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    void UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const QString&);
    void DeleteReviewSession(const QString&);

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

    [[nodiscard]] std::optional<QString> TryReadDefaultReviewSessionIdByRootDeckId(const QString&);
    [[nodiscard]] std::optional<QString> TryReadReviewSessionIdByReviewSessionDefinitionKey(const QString&);
    [[nodiscard]] std::optional<RecoverableReviewSessionMutationErrorEnum>
    CreateCustomReviewSessionDeckSelection(const QString&, const QString&, ReviewSessionDeckSelectionTypeEnum);
    void DeleteCustomReviewSessionDeckSelections(const QString&);
    [[nodiscard]] std::optional<RecoverableReviewSessionMutationErrorEnum> HandleRecoverableReviewSessionMutationError(duckdb::QueryResult&) const;
};

}
