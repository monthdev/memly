#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "Infrastructure/Sql/SqlExecutionGuard.hpp"
#include "Runtime/Crash.hpp"

namespace Infrastructure::Store::ReviewSession {

[[nodiscard]] std::expected<std::string, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateOrReadExistingDefaultReviewSession(const std::string& RootDeckId, const std::string& ReviewSessionDefinitionKey) {
    if (const std::optional<std::string> ExistingReviewSessionId{ TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionId.has_value()) {
        return ExistingReviewSessionId.value();
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateDefaultReviewSessionPreparedStatement->Execute(ReviewSessionDefinitionKey, RootDeckId) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Default review session creation did not insert a review session");
    return (*QueryResult->begin()).GetValue<std::string>(0);
}

[[nodiscard]] std::expected<std::string, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateOrReadExistingCustomReviewSession(const std::string& ReviewSessionName,
                                                            const std::string& ReviewSessionDefinitionKey,
                                                            const std::vector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector) {
    if (const std::optional<std::string> ExistingReviewSessionId{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionId.has_value()) {
        return ExistingReviewSessionId.value();
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateCustomReviewSessionPreparedStatement->Execute(ReviewSessionName, ReviewSessionDefinitionKey) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    const std::string NewCustomReviewSessionId{ (*QueryResult->begin()).GetValue<std::string>(0) };
    for (const ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionVector) {
        RecoverableReviewSessionMutationError = CreateCustomReviewSessionDeckSelection(
            NewCustomReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationError.value() };
        }
    }
    return NewCustomReviewSessionId;
}

[[nodiscard]] std::optional<ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameReviewSessionPreparedStatement->Execute(ReviewSessionName, ReviewSessionId) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return RecoverableReviewSessionMutationError;
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session rename did not update a review session");
    return std::nullopt;
}

[[nodiscard]] std::expected<std::string, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                               const std::string& RootDeckId,
                                               const std::string& ReviewSessionDefinitionKey) {
    if (const std::optional<std::string> ExistingReviewSessionId{ TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionId.has_value()) {
        if (ExistingReviewSessionId.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionToDefaultPreparedStatement->Execute(
        RootDeckId, ReviewSessionDefinitionKey, CurrentReviewSessionId) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session edit to default did not update a review session");
    DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    return CurrentReviewSessionId;
}

[[nodiscard]] std::expected<std::string, ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::EditReviewSessionToCustom(const std::string& CurrentReviewSessionId,
                                              const std::string& ReviewSessionDefinitionKey,
                                              const std::vector<ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector) {
    if (const std::optional<std::string> ExistingReviewSessionId{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionId.has_value()) {
        if (ExistingReviewSessionId.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionToCustomPreparedStatement->Execute(ReviewSessionDefinitionKey,
                                                                                                              CurrentReviewSessionId) };
    std::optional<RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{ HandleRecoverableReviewSessionMutationError(*QueryResult) };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session edit to custom did not update a review session");
    DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    for (const ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionVector) {
        RecoverableReviewSessionMutationError =
            CreateCustomReviewSessionDeckSelection(CurrentReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationError.value() };
        }
    }
    return CurrentReviewSessionId;
}

void ReviewSessionStore::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement->Execute(ReviewSessionId) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session last card review timestamp update did not update a review session");
}

void ReviewSessionStore::DeleteReviewSession(const std::string& ReviewSessionId) {
    DeleteCustomReviewSessionDeckSelections(ReviewSessionId);
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteReviewSessionPreparedStatement->Execute(ReviewSessionId) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Review session delete did not delete a review session");
}

[[nodiscard]] std::optional<std::string> ReviewSessionStore::TryReadDefaultReviewSessionIdByRootDeckId(const std::string& RootDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement->Execute(RootDeckId) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    if (const auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end()) {
        return (*QueryResultIterator).GetValue<std::string>(0);
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<std::string> ReviewSessionStore::TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string& ReviewSessionDefinitionKey) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement->Execute(ReviewSessionDefinitionKey) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    if (const auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end()) {
        return (*QueryResultIterator).GetValue<std::string>(0);
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<ReviewSessionStore::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionStore::CreateCustomReviewSessionDeckSelection(const std::string& ReviewSessionId,
                                                           const std::string& DeckId,
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
        ReviewSessionId,
        DeckId,
        duckdb::string_t{ ReviewSessionDeckSelectionTypeString.data(), static_cast<std::uint32_t>(ReviewSessionDeckSelectionTypeString.size()) }) };
    return HandleRecoverableReviewSessionMutationError(*QueryResult);
}

void ReviewSessionStore::DeleteCustomReviewSessionDeckSelections(const std::string& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement->Execute(ReviewSessionId) };
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
