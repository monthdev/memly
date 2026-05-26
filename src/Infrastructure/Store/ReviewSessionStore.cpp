#include "Infrastructure/Store/ReviewSessionStore.hpp"

#include <duckdb.hpp>

#include <expected>
#include <memory>
#include <string_view>

#include "Infrastructure/Sql/MutationSqlResource.hpp"
#include "Infrastructure/Sql/QuerySqlResource.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"
#include "Runtime/Crash.hpp"

namespace Infrastructure::Store {

namespace {
std::string_view ReviewSessionDeckSelectionTypeToString(const ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum ReviewSessionDeckSelectionType) {
    switch (ReviewSessionDeckSelectionType) {
    case ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum::Self:
        return "self";
    case ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum::Subtree:
        return "subtree";
    case ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum::ExcludeSelf:
        return "exclude_self";
    case ReviewSessionStore::ReviewSessionDeckSelectionTypeEnum::ExcludeSubtree:
        return "exclude_subtree";
    }
}

}

ReviewSessionStore::ReviewSessionStore(duckdb::Connection& DatabaseConnection)
    : m_DatabaseConnection{ DatabaseConnection }
    , m_CreateReviewSessionPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::CreateReviewSessionSql()) }
    , m_CreateReviewSessionDeckSelectionPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::CreateReviewSessionDeckSelectionSql()) }
    , m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement{ m_DatabaseConnection.Prepare(
          Infrastructure::Sql::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql()) }
    , m_DeleteReviewSessionDeckSelectionsPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::DeleteReviewSessionDeckSelectionsSql()) }
    , m_DeleteReviewSessionPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::DeleteReviewSessionSql()) }
    , m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement{ m_DatabaseConnection.Prepare(
          Infrastructure::Sql::ReadReviewSessionIdByReviewSessionDefinitionKeySql()) } {
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateReviewSessionPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateReviewSessionDeckSelectionPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteReviewSessionDeckSelectionsPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteReviewSessionPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement);
}

ReviewSessionStore::~ReviewSessionStore() = default;

[[nodiscard]] std::expected<QString, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateOrReadExistingReviewSession(const QString& ReviewSessionName,
                                                      const QString& ReviewSessionDefinitionKey,
                                                      const QVector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionQVector) {
    m_DatabaseConnection.BeginTransaction();
    try {
        std::expected<QString, RecoverableReviewSessionMutationErrorEnum> ReviewSessionId{ CreateOrReadExistingReviewSessionInCurrentTransaction(
            ReviewSessionName, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionQVector) };
        if (not ReviewSessionId.has_value()) {
            m_DatabaseConnection.Rollback();
            return std::unexpected{ ReviewSessionId.error() };
        }
        m_DatabaseConnection.Commit();
        return ReviewSessionId.value();
    } catch (...) {
        m_DatabaseConnection.Rollback();
        throw;
    }
}

[[nodiscard]] std::expected<QString, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::EditReviewSessionDeckSelections(const QString& CurrentReviewSessionId,
                                                    const QString& ReviewSessionName,
                                                    const QString& ReviewSessionDefinitionKey,
                                                    const QVector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionQVector) {
    m_DatabaseConnection.BeginTransaction();
    try {
        std::expected<QString, RecoverableReviewSessionMutationErrorEnum> ReviewSessionId{ CreateOrReadExistingReviewSessionInCurrentTransaction(
            ReviewSessionName, ReviewSessionDefinitionKey, ReviewSessionDeckSelectionQVector) };
        if (not ReviewSessionId.has_value()) {
            m_DatabaseConnection.Rollback();
            return std::unexpected{ ReviewSessionId.error() };
        }
        if (ReviewSessionId.value() not_eq CurrentReviewSessionId) {
            DeleteReviewSessionInCurrentTransaction(CurrentReviewSessionId);
        }
        m_DatabaseConnection.Commit();
        return ReviewSessionId.value();
    } catch (...) {
        m_DatabaseConnection.Rollback();
        throw;
    }
}

void ReviewSessionStore::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const QString& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement->Execute(
        ReviewSessionId.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session last card review timestamp update did not update a review session");
}

void ReviewSessionStore::DeleteReviewSession(const QString& ReviewSessionId) {
    m_DatabaseConnection.BeginTransaction();
    try {
        DeleteReviewSessionInCurrentTransaction(ReviewSessionId);
        m_DatabaseConnection.Commit();
    } catch (...) {
        m_DatabaseConnection.Rollback();
        throw;
    }
}

[[nodiscard]] std::optional<QString> ReviewSessionStore::TryReadReviewSessionIdByReviewSessionDefinitionKey(const QString& ReviewSessionDefinitionKey) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement->Execute(
        ReviewSessionDefinitionKey.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    if (const auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end()) {
        const auto& QueryResultRow{ *QueryResultIterator };
        return QString{ QueryResultRow.GetValue<std::string>(0).c_str() };
    }
    return std::nullopt;
}

[[nodiscard]] std::expected<QString, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateReviewSession(const QString& ReviewSessionName, const QString& ReviewSessionDefinitionKey) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateReviewSessionPreparedStatement->Execute(ReviewSessionName.toStdString(),
                                                                                                      ReviewSessionDefinitionKey.toStdString()) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    const auto QueryResultIterator{ QueryResult->begin() };
    const auto& QueryResultRow{ *QueryResultIterator };
    return QString{ QueryResultRow.GetValue<std::string>(0).c_str() };
}

[[nodiscard]] std::expected<QString, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateOrReadExistingReviewSessionInCurrentTransaction(const QString& ReviewSessionName,
                                                                          const QString& ReviewSessionDefinitionKey,
                                                                          const QVector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionQVector) {
    if (const std::optional<QString> ExistingReviewSessionId{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionId.has_value()) {
        return ExistingReviewSessionId.value();
    }
    std::expected<QString, RecoverableReviewSessionMutationErrorEnum> NewReviewSessionId{ CreateReviewSession(ReviewSessionName, ReviewSessionDefinitionKey) };
    if (not NewReviewSessionId.has_value()) {
        return std::unexpected{ NewReviewSessionId.error() };
    }
    for (const ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionQVector) {
        std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ CreateReviewSessionDeckSelection(
            NewReviewSessionId.value(), ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType) };
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationError.value() };
        }
    }
    return NewReviewSessionId.value();
}

[[nodiscard]] std::optional<ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateReviewSessionDeckSelection(const QString& ReviewSessionId,
                                                     const QString& DeckId,
                                                     const ReviewSessionDeckSelectionTypeEnum ReviewSessionDeckSelectionType) {
    const std::string_view ReviewSessionDeckSelectionTypeString{ ReviewSessionDeckSelectionTypeToString(ReviewSessionDeckSelectionType) };
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateReviewSessionDeckSelectionPreparedStatement->Execute(
        ReviewSessionId.toStdString(),
        DeckId.toStdString(),
        duckdb::string_t{ ReviewSessionDeckSelectionTypeString.data(), static_cast<uint32_t>(ReviewSessionDeckSelectionTypeString.size()) }) };
    return HandleRecoverableReviewSessionMutationError(*QueryResult);
}

void ReviewSessionStore::DeleteReviewSessionInCurrentTransaction(const QString& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteReviewSessionDeckSelectionsPreparedStatement->Execute(ReviewSessionId.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    QueryResult = m_DeleteReviewSessionPreparedStatement->Execute(ReviewSessionId.toStdString());
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session delete did not delete a review session");
}

[[nodiscard]] std::optional<ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::HandleRecoverableReviewSessionMutationError(duckdb::QueryResult& QueryResult) const {
    if (not QueryResult.HasError()) {
        return std::nullopt;
    }
    const std::string_view ErrorMessage{ QueryResult.GetError() };
    if (ErrorMessage.contains("review_session_name_length_is_valid(\"name\")")) {
        return RecoverableReviewSessionMutationErrorEnum::ReviewSessionNameLengthError;
    }
    if (ErrorMessage.contains("self_selection_conflict")) {
        return RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckSelfSelectionError;
    }
    if (ErrorMessage.contains("subtree_selection_conflict")) {
        return RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckSubtreeSelectionError;
    }
    if (ErrorMessage.contains("include_selection_conflict")) {
        return RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckIncludeSelectionError;
    }
    if (ErrorMessage.contains("exclude_selection_conflict")) {
        return RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckExcludeSelectionError;
    }
    Runtime::ThrowError(QueryResult.GetError());
}

}
