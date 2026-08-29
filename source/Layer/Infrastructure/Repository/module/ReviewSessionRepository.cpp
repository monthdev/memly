module Memly.Repository.ReviewSessionRepository;

// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Repository/ReviewSessionRepository.hpp"

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

#include "Memly/Database/DatabaseRuntime.hpp"
#include "Memly/Database/PreparedStatement.hpp"
#include "Memly/Domain/RecoverableReviewSessionMutationError.hpp"
#include "Memly/Domain/ReviewSessionDeckSelection.hpp"
#include "Memly/Domain/ReviewSessionListRow.hpp"
#include "Memly/Exception/MemlyException.hpp"

namespace Memly::Repository {

ReviewSessionRepository::ReviewSessionRepository(
    Database::DatabaseRuntime& DatabaseRuntime
)
    : m_DatabaseRuntime{ DatabaseRuntime }
    , m_SelectReviewSessionListRowsPreparedStatement{ DatabaseRuntime
            .PrepareStatement(Internal::SelectReviewSessionListRowsSql()) }
    , m_SelectDefaultReviewSessionIdByRootDeckIdPreparedStatement{ DatabaseRuntime
            .PrepareStatement(
                Internal::SelectDefaultReviewSessionIdByRootDeckIdSql()
            ) }
    , m_SelectReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement{ DatabaseRuntime
            .PrepareStatement(
                Internal::SelectReviewSessionIdByReviewSessionDefinitionKeySql()
            ) }
    , m_InsertCustomReviewSessionPreparedStatement{ DatabaseRuntime
            .PrepareStatement(Internal::InsertCustomReviewSessionSql()) }
    , m_InsertDefaultReviewSessionPreparedStatement{ DatabaseRuntime
            .PrepareStatement(Internal::InsertDefaultReviewSessionSql()) }
    , m_InsertCustomReviewSessionDeckSelectionPreparedStatement{ DatabaseRuntime
            .PrepareStatement(
                Internal::InsertCustomReviewSessionDeckSelectionSql()
            ) }
    , m_UpdateReviewSessionNamePreparedStatement{ DatabaseRuntime
            .PrepareStatement(Internal::UpdateReviewSessionNameSql()) }
    , m_UpdateReviewSessionToDefaultPreparedStatement{ DatabaseRuntime
            .PrepareStatement(Internal::UpdateReviewSessionToDefaultSql()) }
    , m_UpdateReviewSessionToCustomPreparedStatement{ DatabaseRuntime
            .PrepareStatement(Internal::UpdateReviewSessionToCustomSql()) }
    , m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement{ DatabaseRuntime
            .PrepareStatement(
                Internal::
                    UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql()
            ) }
    , m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement{ DatabaseRuntime
            .PrepareStatement(
                Internal::DeleteCustomReviewSessionDeckSelectionsSql()
            ) }
    , m_DeleteReviewSessionPreparedStatement{ DatabaseRuntime
            .PrepareStatement(Internal::DeleteReviewSessionSql()) } {
}

namespace {

[[nodiscard]] std::size_t
u_CountResultRows(
    Database::DatabaseRuntime& DatabaseRuntime,
    duckdb::QueryResult& QueryResult,
    const std::source_location SourceLocation = std::source_location::current()
) {
    std::size_t ResultRowCount{ 0 };
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{
        DatabaseRuntime.FetchNextDataChunk(QueryResult, SourceLocation) }) {
        ResultRowCount += DataChunk->size();
    }
    return ResultRowCount;
}

[[maybe_unused,
    nodiscard]] std::optional<Domain::RecoverableReviewSessionMutationErrorEnum>
u_TryGetRecoverableReviewSessionMutationError(
    duckdb::QueryResult& QueryResult
) {
    if (not QueryResult.HasError()) {
        return std::nullopt;
    }
    const std::string& ErrorMessage{ QueryResult.GetError() };
    if (ErrorMessage
            .contains("review_session_custom_name_is_valid(\"custom_name\")")) {
        return Domain::RecoverableReviewSessionMutationErrorEnum::
            ReviewSessionNameLengthError;
    }
    if (ErrorMessage.contains("self_selection_conflict")) {
        return Domain::RecoverableReviewSessionMutationErrorEnum::
            ConflictingReviewSessionDeckSelfSelectionError;
    }
    if (ErrorMessage.contains("subtree_selection_conflict")) {
        return Domain::RecoverableReviewSessionMutationErrorEnum::
            ConflictingReviewSessionDeckSubtreeSelectionError;
    }
    if (ErrorMessage.contains("include_selection_conflict")) {
        return Domain::RecoverableReviewSessionMutationErrorEnum::
            ConflictingReviewSessionDeckIncludeSelectionError;
    }
    if (ErrorMessage.contains("exclude_selection_conflict")) {
        return Domain::RecoverableReviewSessionMutationErrorEnum::
            ConflictingReviewSessionDeckExcludeSelectionError;
    }
    throw Exception::MemlyException{ std::initializer_list<std::string_view>{
        QueryResult.GetError() } };
}

}

[[nodiscard]] std::vector<Domain::ReviewSessionListRow>
ReviewSessionRepository::ReadReviewSessionListRows() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ this->m_DatabaseRuntime
            .ExecutePreparedStatement(this
                    ->m_SelectReviewSessionListRowsPreparedStatement)
            .WithoutParameters() };
    std::vector<Domain::ReviewSessionListRow> ReviewSessionListRowVector{};
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,
    // readability-magic-numbers)
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{
        this->m_DatabaseRuntime.FetchNextDataChunk(*QueryResult) }) {
        for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size();
            ++RowIndex) {
            const duckdb::Value LastUpdatedAtMillisecondsSinceEpochValue{
                DataChunk->GetValue(3, RowIndex)
            };
            const duckdb::Value LastCardReviewAtMillisecondsSinceEpochValue{
                DataChunk->GetValue(4, RowIndex)
            };
            ReviewSessionListRowVector.emplace_back(
                DataChunk->GetValue(0, RowIndex).GetValue<std::string>(),
                DataChunk->GetValue(1, RowIndex).GetValue<std::string>(),
                DataChunk->GetValue(2, RowIndex).GetValue<std::int64_t>(),
                LastUpdatedAtMillisecondsSinceEpochValue.IsNull() ?
                    std::nullopt :
                    std::make_optional(LastUpdatedAtMillisecondsSinceEpochValue
                            .GetValue<std::int64_t>()),
                LastCardReviewAtMillisecondsSinceEpochValue.IsNull() ?
                    std::nullopt :
                    std::make_optional(
                        LastCardReviewAtMillisecondsSinceEpochValue
                            .GetValue<std::int64_t>()
                    )
            );
        }
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,
    // readability-magic-numbers)
    return ReviewSessionListRowVector;
}

[[nodiscard]] std::expected<
    std::string,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionRepository::CreateOrReadExistingDefaultReviewSession(
    const std::string& RootDeckId,
    const std::string& ReviewSessionDefinitionKey
) {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{
            this->TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionIdOptional.has_value()) {
        return ExistingReviewSessionIdOptional.value();
    }
    [[maybe_unused]] const std::size_t ResultRowCount{ u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(this
                    ->m_InsertDefaultReviewSessionPreparedStatement)
            .WithParameters(ReviewSessionDefinitionKey, RootDeckId)
    ) };
    assert(ResultRowCount == 1);
    // std::optional<Domain::RecoverableReviewSessionMutationErrorEnum>
    // RecoverableReviewSessionMutationErrorOptional{
    //     u_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
    //     return std::unexpected{
    //     RecoverableReviewSessionMutationErrorOptional.value() };
    // }
    std::optional<std::string> NewDefaultReviewSessionIdOptional{
        this->TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId)
    };
    assert(NewDefaultReviewSessionIdOptional.has_value());
    return std::move(NewDefaultReviewSessionIdOptional).value();
}

[[nodiscard]] std::expected<
    std::string,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionRepository::CreateOrReadExistingCustomReviewSession(
    const std::string& ReviewSessionName,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSessionDeckSelection>&
        ReviewSessionDeckSelectionVector
) {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{
            this->TryReadReviewSessionIdByReviewSessionDefinitionKey(
                ReviewSessionDefinitionKey
            ) };
        ExistingReviewSessionIdOptional.has_value()) {
        return ExistingReviewSessionIdOptional.value();
    }
    [[maybe_unused]] const std::size_t ResultRowCount{ u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(this
                    ->m_InsertCustomReviewSessionPreparedStatement)
            .WithParameters(ReviewSessionName, ReviewSessionDefinitionKey)
    ) };
    assert(ResultRowCount == 1);
    // std::optional<Domain::RecoverableReviewSessionMutationErrorEnum>
    // RecoverableReviewSessionMutationErrorOptional{
    //     u_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
    //     return std::unexpected{
    //     RecoverableReviewSessionMutationErrorOptional.value() };
    // }
    std::optional<std::string> NewCustomReviewSessionIdOptional{
        this->TryReadReviewSessionIdByReviewSessionDefinitionKey(
            ReviewSessionDefinitionKey
        )
    };
    assert(NewCustomReviewSessionIdOptional.has_value());
    // TODO: The fact that this can't be marked const to allow automatic move at
    // the end suggests separate helper method
    std::string NewCustomReviewSessionId{
        std::move(NewCustomReviewSessionIdOptional).value()
    };
    for (const Domain::ReviewSessionDeckSelection& ReviewSessionDeckSelection :
        ReviewSessionDeckSelectionVector) {
        // RecoverableReviewSessionMutationErrorOptional =
        this->CreateCustomReviewSessionDeckSelection(
            NewCustomReviewSessionId,
            ReviewSessionDeckSelection.m_DeckId,
            ReviewSessionDeckSelection.m_DeckSelectionType
        );
        // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
        //     return std::unexpected{
        //     RecoverableReviewSessionMutationErrorOptional.value() };
        // }
    }
    return NewCustomReviewSessionId;
}

[[nodiscard]] std::optional<Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionRepository::RenameReviewSession(
    const std::string& ReviewSessionId,
    const std::string& ReviewSessionName
) {
    [[maybe_unused]] const std::size_t ResultRowCount{ u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(this
                    ->m_UpdateReviewSessionNamePreparedStatement)
            .WithParameters(ReviewSessionName, ReviewSessionId)
    ) };
    assert(ResultRowCount == 1);
    // std::optional<Domain::RecoverableReviewSessionMutationErrorEnum>
    // RecoverableReviewSessionMutationErrorOptional{
    //     u_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // return RecoverableReviewSessionMutationErrorOptional;
    return std::nullopt;
}

[[nodiscard]] std::expected<
    std::string,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionRepository::EditReviewSessionToDefault(
    const std::string& CurrentReviewSessionId,
    const std::string& RootDeckId,
    const std::string& ReviewSessionDefinitionKey
) {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{
            this->TryReadDefaultReviewSessionIdByRootDeckId(RootDeckId) };
        ExistingReviewSessionIdOptional.has_value()) {
        if (ExistingReviewSessionIdOptional.value() not_eq
            CurrentReviewSessionId) {
            return std::unexpected{
                Domain::RecoverableReviewSessionMutationErrorEnum::
                    DuplicateReviewSessionDefinitionKeyError
            };
        }
    }
    [[maybe_unused]] const std::size_t ResultRowCount{ u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(this
                    ->m_UpdateReviewSessionToDefaultPreparedStatement)
            .WithParameters(
                RootDeckId,
                ReviewSessionDefinitionKey,
                CurrentReviewSessionId
            )
    ) };
    assert(ResultRowCount == 1);
    // std::optional<Domain::RecoverableReviewSessionMutationErrorEnum>
    // RecoverableReviewSessionMutationErrorOptional{
    //     u_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
    //     return std::unexpected{
    //     RecoverableReviewSessionMutationErrorOptional.value() };
    // }
    this->DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    return CurrentReviewSessionId;
}

[[nodiscard]] std::expected<
    std::string,
    Domain::RecoverableReviewSessionMutationErrorEnum>
ReviewSessionRepository::EditReviewSessionToCustom(
    const std::string& CurrentReviewSessionId,
    const std::string& ReviewSessionDefinitionKey,
    const std::vector<Domain::ReviewSessionDeckSelection>&
        ReviewSessionDeckSelectionVector
) {
    if (const std::optional<std::string> ExistingReviewSessionIdOptional{
            this->TryReadReviewSessionIdByReviewSessionDefinitionKey(
                ReviewSessionDefinitionKey
            ) };
        ExistingReviewSessionIdOptional.has_value()) {
        if (ExistingReviewSessionIdOptional.value() not_eq
            CurrentReviewSessionId) {
            return std::unexpected{
                Domain::RecoverableReviewSessionMutationErrorEnum::
                    DuplicateReviewSessionDefinitionKeyError
            };
        }
    }
    [[maybe_unused]] const std::size_t ResultRowCount{ u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(this
                    ->m_UpdateReviewSessionToCustomPreparedStatement)
            .WithParameters(ReviewSessionDefinitionKey, CurrentReviewSessionId)
    ) };
    assert(ResultRowCount == 1);
    // std::optional<Domain::RecoverableReviewSessionMutationErrorEnum>
    // RecoverableReviewSessionMutationErrorOptional{
    //     u_TryGetRecoverableReviewSessionMutationError(*QueryResult)
    // };
    // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
    //     return std::unexpected{
    //     RecoverableReviewSessionMutationErrorOptional.value() };
    // }
    this->DeleteCustomReviewSessionDeckSelections(CurrentReviewSessionId);
    for (const Domain::ReviewSessionDeckSelection& ReviewSessionDeckSelection :
        ReviewSessionDeckSelectionVector) {
        // RecoverableReviewSessionMutationErrorOptional =
        this->CreateCustomReviewSessionDeckSelection(
            CurrentReviewSessionId,
            ReviewSessionDeckSelection.m_DeckId,
            ReviewSessionDeckSelection.m_DeckSelectionType
        );
        // if (RecoverableReviewSessionMutationErrorOptional.has_value()) {
        //     return std::unexpected{
        //     RecoverableReviewSessionMutationErrorOptional.value() };
        // }
    }
    return CurrentReviewSessionId;
}

void
ReviewSessionRepository::
    UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(
        const std::string& ReviewSessionId
    ) {
    [[maybe_unused]] const std::size_t ResultRowCount{ u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(
                this->m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement
            )
            .WithParameters(ReviewSessionId)
    ) };
    assert(ResultRowCount == 1);
}

void
ReviewSessionRepository::DeleteReviewSession(
    const std::string& ReviewSessionId
) {
    this->DeleteCustomReviewSessionDeckSelections(ReviewSessionId);
    [[maybe_unused]] const std::size_t ResultRowCount{ u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(this
                    ->m_DeleteReviewSessionPreparedStatement)
            .WithParameters(ReviewSessionId)
    ) };
    assert(ResultRowCount == 1);
}

namespace {

[[nodiscard]] std::optional<std::string>
u_TryReadSingleStringResult(
    Database::DatabaseRuntime& DatabaseRuntime,
    duckdb::QueryResult& QueryResult,
    const std::source_location SourceLocation = std::source_location::current()
) {
    std::optional<std::string> ResultOptional{};
    std::size_t ResultRowCount{ 0 };
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{
        DatabaseRuntime.FetchNextDataChunk(QueryResult, SourceLocation) }) {
        for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size();
            ++RowIndex) {
            ++ResultRowCount;
            ResultOptional =
                DataChunk->GetValue(0, RowIndex).GetValue<std::string>();
        }
    }
    assert(ResultRowCount <= 1);
    return ResultOptional;
}

}

[[nodiscard]] std::optional<std::string>
ReviewSessionRepository::TryReadDefaultReviewSessionIdByRootDeckId(
    const std::string& RootDeckId
) {
    return u_TryReadSingleStringResult(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(
                this->m_SelectDefaultReviewSessionIdByRootDeckIdPreparedStatement
            )
            .WithParameters(RootDeckId)
    );
}

[[nodiscard]] std::optional<std::string>
ReviewSessionRepository::TryReadReviewSessionIdByReviewSessionDefinitionKey(
    const std::string& ReviewSessionDefinitionKey
) {
    return u_TryReadSingleStringResult(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(
                this->m_SelectReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement
            )
            .WithParameters(ReviewSessionDefinitionKey)
    );
}

namespace {

[[nodiscard]] constexpr const char*
u_ReviewSessionDeckSelectionTypeToString(
    const Domain::ReviewSessionDeckSelection::DeckSelectionTypeEnum
        DeckSelectionType
) noexcept {
    switch (DeckSelectionType) {
    case Domain::ReviewSessionDeckSelection::DeckSelectionTypeEnum::Self:
        return "self";
    case Domain::ReviewSessionDeckSelection::DeckSelectionTypeEnum::Subtree:
        return "subtree";
    case Domain::ReviewSessionDeckSelection::DeckSelectionTypeEnum::ExcludeSelf:
        return "exclude_self";
    case Domain::ReviewSessionDeckSelection::DeckSelectionTypeEnum::
        ExcludeSubtree:
        return "exclude_subtree";
    default:
        assert(false);
        std::unreachable();
    }
}

}

void
ReviewSessionRepository::CreateCustomReviewSessionDeckSelection(
    const std::string& ReviewSessionId,
    const std::string& DeckId,
    const Domain::ReviewSessionDeckSelection::DeckSelectionTypeEnum
        DeckSelectionType
) {
    const char* const DeckSelectionTypeString{
        u_ReviewSessionDeckSelectionTypeToString(DeckSelectionType)
    };
    [[maybe_unused]] const std::size_t ResultRowCount{ u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(this
                    ->m_InsertCustomReviewSessionDeckSelectionPreparedStatement)
            .WithParameters(ReviewSessionId, DeckId, DeckSelectionTypeString)
    ) };
    assert(ResultRowCount == 1);
    // return u_TryGetRecoverableReviewSessionMutationError(*QueryResult);
}

void
ReviewSessionRepository::DeleteCustomReviewSessionDeckSelections(
    const std::string& ReviewSessionId
) {
    static_cast<void>(u_CountResultRows(
        this->m_DatabaseRuntime,
        *this->m_DatabaseRuntime
            .ExecutePreparedStatement(
                this->m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement
            )
            .WithParameters(ReviewSessionId)
    ));
}

}
#endif
