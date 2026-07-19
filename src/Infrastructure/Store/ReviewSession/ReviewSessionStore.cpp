#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

#include <duckdb.hpp>

#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Support/Runtime/ThrowMemlyException.hpp"

namespace Infrastructure::Store::ReviewSession {
namespace {

[[nodiscard]] constexpr auto
u_ReviewSessionDeckSelectionTypeToString(const Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum DeckSelectionType) noexcept
    -> const char* {
    switch (DeckSelectionType) {
    case Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum::Self:
        return "self";
    case Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum::Subtree:
        return "subtree";
    case Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum::ExcludeSelf:
        return "exclude_self";
    case Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum::ExcludeSubtree:
        return "exclude_subtree";
    }
}

[[nodiscard]] auto u_HandleRecoverableReviewSessionMutationError(duckdb::QueryResult& QueryResult)
    -> std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (not QueryResult.HasError()) {
        return std::nullopt;
    }
    const std::string& ErrorMessage{ QueryResult.GetError() };
    if (ErrorMessage.contains("review_session_custom_name_is_valid(\"custom_name\")")) {
        return Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::ReviewSessionNameLengthError;
    }
    if (ErrorMessage.contains("self_selection_conflict")) {
        return Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckSelfSelectionError;
    }
    if (ErrorMessage.contains("subtree_selection_conflict")) {
        return Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckSubtreeSelectionError;
    }
    if (ErrorMessage.contains("include_selection_conflict")) {
        return Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckIncludeSelectionError;
    }
    if (ErrorMessage.contains("exclude_selection_conflict")) {
        return Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::ConflictingReviewSessionDeckExcludeSelectionError;
    }
    Support::Runtime::ThrowMemlyException(QueryResult.GetError());
}

}

[[nodiscard]] auto ReviewSessionStore::CreateOrReadExistingDefaultReviewSession(const std::string& RootDeckId, const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionId{ TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionId.has_value()) {
        return ExistingReviewSessionId.value();
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateDefaultReviewSessionPreparedStatement->Execute(ReviewSessionDefinitionKey, RootDeckId) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{
        u_HandleRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Database::ThrowOnMutationNoOp(*QueryResult, "Default review session creation did not insert a review session");
    return (*QueryResult->begin()).GetValue<std::string>(0);
}

[[nodiscard]] auto ReviewSessionStore::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionId{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionId.has_value()) {
        return ExistingReviewSessionId.value();
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateCustomReviewSessionPreparedStatement->Execute(ReviewSessionName, ReviewSessionDefinitionKey) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{
        u_HandleRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    const std::string NewCustomReviewSessionId{ (*QueryResult->begin()).GetValue<std::string>(0) };
    for (const Application::Domain::ReviewSession::ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionVector) {
        RecoverableReviewSessionMutationError = CreateCustomReviewSessionDeckSelection(
            NewCustomReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        if (RecoverableReviewSessionMutationError.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationError.value() };
        }
    }
    return NewCustomReviewSessionId;
}

[[nodiscard]] auto ReviewSessionStore::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName)
    -> std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameReviewSessionPreparedStatement->Execute(ReviewSessionName, ReviewSessionId) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{
        u_HandleRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return RecoverableReviewSessionMutationError;
    }
    Infrastructure::Database::ThrowOnMutationNoOp(*QueryResult, "Review session rename did not update a review session");
    return std::nullopt;
}

[[nodiscard]] auto ReviewSessionStore::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                                                  const std::string& RootDeckId,
                                                                  const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionId{ TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionId.has_value()) {
        if (ExistingReviewSessionId.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionToDefaultPreparedStatement->Execute(
        RootDeckId, ReviewSessionDefinitionKey, CurrentReviewSessionId) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{
        u_HandleRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Database::ThrowOnMutationNoOp(*QueryResult, "Review session edit to default did not update a review session");
    DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    return CurrentReviewSessionId;
}

[[nodiscard]] auto ReviewSessionStore::EditReviewSessionToCustom(
    const std::string& CurrentReviewSessionId,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionId{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionId.has_value()) {
        if (ExistingReviewSessionId.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionToCustomPreparedStatement->Execute(ReviewSessionDefinitionKey,
                                                                                                              CurrentReviewSessionId) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationError{
        u_HandleRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationError.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationError.value() };
    }
    Infrastructure::Database::ThrowOnMutationNoOp(*QueryResult, "Review session edit to custom did not update a review session");
    DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    for (const Application::Domain::ReviewSession::ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionVector) {
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
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Database::ThrowOnMutationNoOp(*QueryResult, "Review session last card review timestamp update did not update a review session");
}

void ReviewSessionStore::DeleteReviewSession(const std::string& ReviewSessionId) {
    DeleteCustomReviewSessionDeckSelections(ReviewSessionId);
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteReviewSessionPreparedStatement->Execute(ReviewSessionId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Database::ThrowOnMutationNoOp(*QueryResult, "Review session delete did not delete a review session");
}

[[nodiscard]] auto ReviewSessionStore::TryReadDefaultReviewSessionIdByRootDeckId(const std::string& RootDeckId) -> std::optional<std::string> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement->Execute(RootDeckId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    if (const auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end()) {
        return (*QueryResultIterator).GetValue<std::string>(0);
    }
    return std::nullopt;
}

[[nodiscard]] auto ReviewSessionStore::TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string& ReviewSessionDefinitionKey)
    -> std::optional<std::string> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement->Execute(ReviewSessionDefinitionKey) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    if (const auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end()) {
        return (*QueryResultIterator).GetValue<std::string>(0);
    }
    return std::nullopt;
}

[[nodiscard]] auto ReviewSessionStore::CreateCustomReviewSessionDeckSelection(
    const std::string& ReviewSessionId,
    const std::string& DeckId,
    const Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum DeckSelectionType)
    -> std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    const char* const DeckSelectionTypeString{ u_ReviewSessionDeckSelectionTypeToString(DeckSelectionType) };
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateCustomReviewSessionDeckSelectionPreparedStatement->Execute(
        ReviewSessionId, DeckId, DeckSelectionTypeString) };
    return u_HandleRecoverableReviewSessionMutationError(*QueryResult);
}

void ReviewSessionStore::DeleteCustomReviewSessionDeckSelections(const std::string& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement->Execute(ReviewSessionId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
}

}
