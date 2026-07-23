#include "Infrastructure/Store/ReviewSession/ReviewSessionStore.hpp"

#include <duckdb.hpp>

#include <cassert>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Support/Runtime/ThrowMemlyException.hpp"

namespace Infrastructure::Store::ReviewSession {
namespace {

[[nodiscard]] constexpr auto
a_ReviewSessionDeckSelectionTypeToString(const Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum DeckSelectionType) noexcept
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
    default:
        assert(false);
        std::unreachable();
    }
}

[[nodiscard]] auto a_TryGetRecoverableReviewSessionMutationError(duckdb::QueryResult& QueryResult)
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
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{ TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionIdOptional.has_value()) {
        return ExistingReviewSessionIdOptional.value();
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateDefaultReviewSessionPreparedStatement->Execute(ReviewSessionDefinitionKey, RootDeckId) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
        a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
    }
    assert(QueryResult->begin() not_eq QueryResult->end());
    return (*QueryResult->begin()).GetValue<std::string>(0);
}

[[nodiscard]] auto ReviewSessionStore::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionIdOptional.has_value()) {
        return ExistingReviewSessionIdOptional.value();
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateCustomReviewSessionPreparedStatement->Execute(ReviewSessionName, ReviewSessionDefinitionKey) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
        a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
    }
    // TODO: The fact that this can't be marked const to allow automatic move at the end suggests separate helper method
    std::string NewCustomReviewSessionId{ (*QueryResult->begin()).GetValue<std::string>(0) };
    for (const Application::Domain::ReviewSession::ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionVector) {
        RecoverableReviewSessionMutationErrorOptional = CreateCustomReviewSessionDeckSelection(
            NewCustomReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
        }
    }
    return NewCustomReviewSessionId;
}

[[nodiscard]] auto ReviewSessionStore::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName)
    -> std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameReviewSessionPreparedStatement->Execute(ReviewSessionName, ReviewSessionId) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
        a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (not RecoverableReviewSessionMutationErrorOptional.has_value()) {
        assert(QueryResult->begin() not_eq QueryResult->end());
    }
    return RecoverableReviewSessionMutationErrorOptional;
}

[[nodiscard]] auto ReviewSessionStore::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                                                  const std::string& RootDeckId,
                                                                  const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{ TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionIdOptional.has_value()) {
        if (ExistingReviewSessionIdOptional.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionToDefaultPreparedStatement->Execute(
        RootDeckId, ReviewSessionDefinitionKey, CurrentReviewSessionId) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
        a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
    }
    assert(QueryResult->begin() not_eq QueryResult->end());
    DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    return CurrentReviewSessionId;
}

[[nodiscard]] auto ReviewSessionStore::EditReviewSessionToCustom(
    const std::string& CurrentReviewSessionId,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{ TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
        ExistingReviewSessionIdOptional.has_value()) {
        if (ExistingReviewSessionIdOptional.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionToCustomPreparedStatement->Execute(ReviewSessionDefinitionKey,
                                                                                                              CurrentReviewSessionId) };
    std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
        a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    };
    if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
        return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
    }
    assert(QueryResult->begin() not_eq QueryResult->end());
    DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    for (const Application::Domain::ReviewSession::ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionVector) {
        RecoverableReviewSessionMutationErrorOptional =
            CreateCustomReviewSessionDeckSelection(CurrentReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
            return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
        }
    }
    return CurrentReviewSessionId;
}

void ReviewSessionStore::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement->Execute(ReviewSessionId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    assert(QueryResult->begin() not_eq QueryResult->end());
}

void ReviewSessionStore::DeleteReviewSession(const std::string& ReviewSessionId) {
    DeleteCustomReviewSessionDeckSelections(ReviewSessionId);
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteReviewSessionPreparedStatement->Execute(ReviewSessionId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    assert(QueryResult->begin() not_eq QueryResult->end());
}

[[nodiscard]] auto ReviewSessionStore::TryReadDefaultReviewSessionIdByRootDeckId(const std::string& RootDeckId) -> std::optional<std::string> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement->Execute(RootDeckId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
    if (const auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end()) {
        return (*QueryResultIterator).GetValue<std::string>(0);
    }
    return std::nullopt;
}

[[nodiscard]] auto ReviewSessionStore::TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string& ReviewSessionDefinitionKey)
    -> std::optional<std::string> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement->Execute(ReviewSessionDefinitionKey) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
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
    const char* const DeckSelectionTypeString{ a_ReviewSessionDeckSelectionTypeToString(DeckSelectionType) };
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateCustomReviewSessionDeckSelectionPreparedStatement->Execute(
        ReviewSessionId, DeckId, DeckSelectionTypeString) };
    return a_TryGetRecoverableReviewSessionMutationError(*QueryResult);
}

void ReviewSessionStore::DeleteCustomReviewSessionDeckSelections(const std::string& ReviewSessionId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement->Execute(ReviewSessionId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
}

}
