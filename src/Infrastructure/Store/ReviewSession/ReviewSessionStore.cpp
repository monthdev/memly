#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

#include <duckdb.hpp>

#include <expected>
#include <memory>
#include <string_view>

#include "Infrastructure/Sql/Mutation/ReviewSession/ReviewSessionMutationSql.hpp"
#include "Infrastructure/Sql/Query/ReviewSession/ReviewSessionQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"
#include "Runtime/Crash.hpp"

namespace Infrastructure::Store::ReviewSession {

ReviewSessionStore::ReviewSessionStore(duckdb::Connection& DatabaseConnection)
    : m_CreateCustomReviewSessionPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::ReviewSession::CreateCustomReviewSessionSql()) }
    , m_CreateDefaultReviewSessionPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::ReviewSession::CreateDefaultReviewSessionSql()) }
    , m_CreateCustomReviewSessionDeckSelectionPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::Mutation::ReviewSession::CreateCustomReviewSessionDeckSelectionSql()) }
    , m_RenameReviewSessionPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::ReviewSession::RenameReviewSessionSql()) }
    , m_UpdateReviewSessionToDefaultPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::Mutation::ReviewSession::UpdateReviewSessionToDefaultSql()) }
    , m_UpdateReviewSessionToCustomPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::Mutation::ReviewSession::UpdateReviewSessionToCustomSql()) }
    , m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::Mutation::ReviewSession::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql()) }
    , m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::Mutation::ReviewSession::DeleteCustomReviewSessionDeckSelectionsSql()) }
    , m_DeleteReviewSessionPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::ReviewSession::DeleteReviewSessionSql()) }
    , m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::Query::ReviewSession::ReadDefaultReviewSessionIdByRootDeckIdSql()) }
    , m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::Query::ReviewSession::ReadReviewSessionIdByReviewSessionDefinitionKeySql()) } {
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

ReviewSessionStore::~ReviewSessionStore() = default;

[[nodiscard]] std::expected<QString, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateOrReadExistingDefaultReviewSession(const QString& RootDeckId, const QString& ReviewSessionDefinitionKey) {
    if (const std::optional<QString> ExistingReviewSessionId{ TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) }; ExistingReviewSessionId.has_value()) {
        return ExistingReviewSessionId.value();
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateDefaultReviewSessionPreparedStatement->Execute(ReviewSessionDefinitionKey.toStdString(),
                                                                                                             RootDeckId.toStdString()) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Default review session creation did not insert a review session");
    return QString{ (*QueryResult->begin()).GetValue<std::string>(0).c_str() };
}

[[nodiscard]] std::expected<QString, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateOrReadExistingCustomReviewSession(const QString& ReviewSessionName,
                                                            const QString& ReviewSessionDefinitionKey,
                                                            const QVector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionQVector) {
    if (const std::optional<QString> ExistingReviewSessionId{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionId.has_value()) {
        return ExistingReviewSessionId.value();
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateCustomReviewSessionPreparedStatement->Execute(ReviewSessionName.toStdString(),
                                                                                                            ReviewSessionDefinitionKey.toStdString()) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    const QString NewCustomReviewSessionId{ (*QueryResult->begin()).GetValue<std::string>(0).c_str() };
    for (const ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionQVector) {
        RecoverableReviewSessionMutationError = CreateCustomReviewSessionDeckSelection(
            NewCustomReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationError.value() };
        }
    }
    return NewCustomReviewSessionId;
}

[[nodiscard]] std::optional<ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::RenameReviewSession(const QString& ReviewSessionId, const QString& ReviewSessionName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameReviewSessionPreparedStatement->Execute(ReviewSessionName.toStdString(),
                                                                                                      ReviewSessionId.toStdString()) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return RecoverableReviewSessionMutationError;
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session rename did not update a review session");
    return std::nullopt;
}

[[nodiscard]] std::expected<QString, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::EditReviewSessionToDefault(const QString& CurrentReviewSessionId, const QString& RootDeckId, const QString& ReviewSessionDefinitionKey) {
    if (const std::optional<QString> ExistingReviewSessionId{ TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) }; ExistingReviewSessionId.has_value()) {
        if (ExistingReviewSessionId.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionToDefaultPreparedStatement->Execute(
        RootDeckId.toStdString(), ReviewSessionDefinitionKey.toStdString(), CurrentReviewSessionId.toStdString()) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session edit to default did not update a review session");
    DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    return CurrentReviewSessionId;
}

[[nodiscard]] std::expected<QString, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::EditReviewSessionToCustom(const QString& CurrentReviewSessionId,
                                              const QString& ReviewSessionDefinitionKey,
                                              const QVector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionQVector) {
    if (const std::optional<QString> ExistingReviewSessionId{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionId.has_value()) {
        if (ExistingReviewSessionId.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionToCustomPreparedStatement->Execute(ReviewSessionDefinitionKey.toStdString(),
                                                                                                              CurrentReviewSessionId.toStdString()) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session edit to custom did not update a review session");
    DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    for (const ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionQVector) {
        RecoverableReviewSessionMutationError =
            CreateCustomReviewSessionDeckSelection(CurrentReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationError.value() };
        }
    }
    return CurrentReviewSessionId;
}

void ReviewSessionStore::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const QString& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement->Execute(
        ReviewSessionId.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session last card review timestamp update did not update a review session");
}

void ReviewSessionStore::DeleteReviewSession(const QString& ReviewSessionId) {
    DeleteCustomReviewSessionDeckSelections(ReviewSessionId);
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteReviewSessionPreparedStatement->Execute(ReviewSessionId.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session delete did not delete a review session");
}

[[nodiscard]] std::optional<QString> ReviewSessionStore::TryReadDefaultReviewSessionIdByRootDeckId(const QString& RootDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement->Execute(RootDeckId.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    if (const auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end()) {
        return QString{ (*QueryResultIterator).GetValue<std::string>(0).c_str() };
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<QString> ReviewSessionStore::TryReadReviewSessionIdByReviewSessionDefinitionKey(const QString& ReviewSessionDefinitionKey) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement->Execute(
        ReviewSessionDefinitionKey.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    if (const auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end()) {
        return QString{ (*QueryResultIterator).GetValue<std::string>(0).c_str() };
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateCustomReviewSessionDeckSelection(const QString& ReviewSessionId,
                                                           const QString& DeckId,
                                                           const ReviewSessionDeckSelectionTypeEnum ReviewSessionDeckSelectionType) {
    std::string_view ReviewSessionDeckSelectionTypeString;
    switch (ReviewSessionDeckSelectionType) {
    case ReviewSessionDeckSelectionTypeEnum::Self:
        ReviewSessionDeckSelectionTypeString = "self";
        break;
    case ReviewSessionDeckSelectionTypeEnum::Subtree:
        ReviewSessionDeckSelectionTypeString = "subtree";
        break;
    case ReviewSessionDeckSelectionTypeEnum::ExcludeSelf:
        ReviewSessionDeckSelectionTypeString = "exclude_self";
        break;
    case ReviewSessionDeckSelectionTypeEnum::ExcludeSubtree:
        ReviewSessionDeckSelectionTypeString = "exclude_subtree";
        break;
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateCustomReviewSessionDeckSelectionPreparedStatement->Execute(
        ReviewSessionId.toStdString(),
        DeckId.toStdString(),
        duckdb::string_t{ ReviewSessionDeckSelectionTypeString.data(), static_cast<uint32_t>(ReviewSessionDeckSelectionTypeString.size()) }) };
    return HandleRecoverableReviewSessionMutationError(*QueryResult);
}

void ReviewSessionStore::DeleteCustomReviewSessionDeckSelections(const QString& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement->Execute(ReviewSessionId.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
}

[[nodiscard]] std::optional<ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::HandleRecoverableReviewSessionMutationError(duckdb::QueryResult& QueryResult) const {
    if (not QueryResult.HasError()) {
        return std::nullopt;
    }
    const std::string_view ErrorMessage{ QueryResult.GetError() };
    if (ErrorMessage.contains("review_session_custom_name_is_valid(\"custom_name\")")) {
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
