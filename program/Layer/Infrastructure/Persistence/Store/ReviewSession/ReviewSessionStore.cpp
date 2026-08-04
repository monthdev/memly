// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Infrastructure/Persistence/Store/ReviewSession/ReviewSessionStore.hpp"

#include <duckdb.hpp>

#include <cassert>
#include <cstddef>
#include <expected>
#include <initializer_list>
#include <memory>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Layer/Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Layer/Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Layer/Infrastructure/Persistence/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Persistence/Database/PreparedStatement.hpp"
#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Infrastructure::Persistence::Store::ReviewSession {

namespace {

[[nodiscard]] auto a_CountResultRows(Database::DatabaseRuntime& DatabaseRuntime,
                                     duckdb::QueryResult& QueryResult,
                                     const std::source_location& SourceLocation = std::source_location::current()) -> std::size_t {
    std::size_t ResultRowCount{ 0 };
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ DatabaseRuntime.FetchNextDataChunk(QueryResult, SourceLocation) }) {
        ResultRowCount += DataChunk->size();
    }
    return ResultRowCount;
}

[[maybe_unused, nodiscard]] auto a_TryGetRecoverableReviewSessionMutationError(duckdb::QueryResult& QueryResult)
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
    Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ QueryResult.GetError() });
}

}

[[nodiscard]] auto ReviewSessionStore::CreateOrReadExistingDefaultReviewSession(const std::string& RootDeckId, const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{ this->TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionIdOptional.has_value()) {
        return ExistingReviewSessionIdOptional.value();
    }
    [[maybe_unused]] const std::size_t ResultRowCount{ a_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_CreateDefaultReviewSessionPreparedStatement)
             .WithParameters(ReviewSessionDefinitionKey, RootDeckId)) };
    assert(ResultRowCount == 1);
    // std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
    //     a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
    //     return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
    // }
    std::optional<std::string> NewDefaultReviewSessionIdOptional{ this->TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
    assert(NewDefaultReviewSessionIdOptional.has_value());
    return std::move(NewDefaultReviewSessionIdOptional).value();
}

[[nodiscard]] auto ReviewSessionStore::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{ this->TryReadReviewSessionIdByReviewSessionDefinitionKey(
            ReviewSessionDefinitionKey) };
        ExistingReviewSessionIdOptional.has_value()) {
        return ExistingReviewSessionIdOptional.value();
    }
    [[maybe_unused]] const std::size_t ResultRowCount{ a_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_CreateCustomReviewSessionPreparedStatement)
             .WithParameters(ReviewSessionName, ReviewSessionDefinitionKey)) };
    assert(ResultRowCount == 1);
    // std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
    //     a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
    //     return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
    // }
    std::optional<std::string> NewCustomReviewSessionIdOptional{ this->TryReadReviewSessionIdByReviewSessionDefinitionKey(ReviewSessionDefinitionKey) };
    assert(NewCustomReviewSessionIdOptional.has_value());
    // TODO: The fact that this can't be marked const to allow automatic move at the end suggests separate helper method
    std::string NewCustomReviewSessionId{ std::move(NewCustomReviewSessionIdOptional).value() };
    for (const Application::Domain::ReviewSession::ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionVector) {
        // RecoverableReviewSessionMutationErrorOptional =
        this->CreateCustomReviewSessionDeckSelection(
            NewCustomReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
        //     return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
        // }
    }
    return NewCustomReviewSessionId;
}

[[nodiscard]] auto ReviewSessionStore::RenameReviewSession(const std::string& ReviewSessionId, const std::string& ReviewSessionName)
    -> std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    [[maybe_unused]] const std::size_t ResultRowCount{ a_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_RenameReviewSessionPreparedStatement).WithParameters(ReviewSessionName, ReviewSessionId)) };
    assert(ResultRowCount == 1);
    // std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
    //     a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // return RecoverableReviewSessionMutationErrorOptional;
    return std::nullopt;
}

[[nodiscard]] auto ReviewSessionStore::EditReviewSessionToDefault(const std::string& CurrentReviewSessionId,
                                                                  const std::string& RootDeckId,
                                                                  const std::string& ReviewSessionDefinitionKey)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{ this->TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionIdOptional.has_value()) {
        if (ExistingReviewSessionIdOptional.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    [[maybe_unused]] const std::size_t ResultRowCount{ a_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_UpdateReviewSessionToDefaultPreparedStatement).WithParameters(
            RootDeckId, ReviewSessionDefinitionKey, CurrentReviewSessionId)) };
    assert(ResultRowCount == 1);
    // std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
    //     a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
    //     return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
    // }
    this->DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    return CurrentReviewSessionId;
}

[[nodiscard]] auto ReviewSessionStore::EditReviewSessionToCustom(
    const std::string& CurrentReviewSessionId,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>& ReviewSessionDeckSelectionVector)
    -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{ this->TryReadReviewSessionIdByReviewSessionDefinitionKey(
            ReviewSessionDefinitionKey) };
        ExistingReviewSessionIdOptional.has_value()) {
        if (ExistingReviewSessionIdOptional.value() not_eq CurrentReviewSessionId) {
            return std::unexpected{ Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum::DuplicateReviewSessionDefinitionKeyError };
        }
    }
    [[maybe_unused]] const std::size_t ResultRowCount{ a_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_UpdateReviewSessionToCustomPreparedStatement)
             .WithParameters(ReviewSessionDefinitionKey, CurrentReviewSessionId)) };
    assert(ResultRowCount == 1);
    // std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum> RecoverableReviewSessionMutationErrorOptional{
    //     a_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
    //     return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
    // }
    this->DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    for (const Application::Domain::ReviewSession::ReviewSessionDeckSelection& ReviewSessionDeckSelection : ReviewSessionDeckSelectionVector) {
        // RecoverableReviewSessionMutationErrorOptional =
        this->CreateCustomReviewSessionDeckSelection(
            CurrentReviewSessionId, ReviewSessionDeckSelection.m_DeckId, ReviewSessionDeckSelection.m_DeckSelectionType);
        // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
        //     return std::unexpected{ RecoverableReviewSessionMutationErrorOptional.value() };
        // }
    }
    return CurrentReviewSessionId;
}

void ReviewSessionStore::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string& ReviewSessionId) {
    [[maybe_unused]] const std::size_t ResultRowCount{ a_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement)
             .WithParameters(ReviewSessionId)) };
    assert(ResultRowCount == 1);
}

void ReviewSessionStore::DeleteReviewSession(const std::string& ReviewSessionId) {
    this->DeleteCustomReviewSessionDeckSelections(ReviewSessionId);
    [[maybe_unused]] const std::size_t ResultRowCount{ a_CountResultRows(
        this->m_DatabaseRuntime, *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_DeleteReviewSessionPreparedStatement).WithParameters(ReviewSessionId)) };
    assert(ResultRowCount == 1);
}

namespace {

[[nodiscard]] auto a_TryReadSingleStringResult(Database::DatabaseRuntime& DatabaseRuntime,
                                               duckdb::QueryResult& QueryResult,
                                               const std::source_location& SourceLocation = std::source_location::current()) -> std::optional<std::string> {
    std::optional<std::string> ResultOptional{};
    std::size_t ResultRowCount{ 0 };
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ DatabaseRuntime.FetchNextDataChunk(QueryResult, SourceLocation) }) {
        for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size(); ++RowIndex) {
            ++ResultRowCount;
            ResultOptional = DataChunk->GetValue(0, RowIndex).GetValue<std::string>();
        }
    }
    assert(ResultRowCount <= 1);
    return ResultOptional;
}

}

[[nodiscard]] auto ReviewSessionStore::TryReadDefaultReviewSessionIdByRootDeckId(const std::string& RootDeckId) -> std::optional<std::string> {
    return a_TryReadSingleStringResult(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement).WithParameters(RootDeckId));
}

[[nodiscard]] auto ReviewSessionStore::TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string& ReviewSessionDefinitionKey)
    -> std::optional<std::string> {
    return a_TryReadSingleStringResult(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement)
             .WithParameters(ReviewSessionDefinitionKey));
}

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

}

void ReviewSessionStore::CreateCustomReviewSessionDeckSelection(
    const std::string& ReviewSessionId,
    const std::string& DeckId,
    const Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum DeckSelectionType) {
    const char* const DeckSelectionTypeString{ a_ReviewSessionDeckSelectionTypeToString(DeckSelectionType) };
    [[maybe_unused]] const std::size_t ResultRowCount{ a_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_CreateCustomReviewSessionDeckSelectionPreparedStatement).WithParameters(
            ReviewSessionId, DeckId, DeckSelectionTypeString)) };
    assert(ResultRowCount == 1);
    // return a_TryGetRecoverableReviewSessionMutationError(*QueryResult);
}

void ReviewSessionStore::DeleteCustomReviewSessionDeckSelections(const std::string& ReviewSessionId) {
    static_cast<void>(a_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement).WithParameters(ReviewSessionId)));
}

}
#endif
