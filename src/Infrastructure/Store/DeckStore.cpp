#include "Infrastructure/Store/DeckStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Infrastructure/Sql/MutationSqlResource.hpp"
#include "Infrastructure/Store/QueryResultGuard.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

DeckStore::DeckStore(duckdb::Connection& DatabaseConnection)
    : m_DatabaseConnection{ DatabaseConnection }
    , m_CreateRootDeckPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::CreateRootDeckSql()) }
    , m_CreateChildDeckPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::CreateChildDeckSql()) }
    , m_MoveDeckPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::MoveDeckSql()) }
    , m_RenameDeckPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::RenameDeckSql()) }
    , m_DeleteDeckCardReviewsPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::DeleteDeckCardReviewsSql()) }
    , m_DeleteDeckCardsPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::DeleteDeckCardsSql()) }
    , m_DeleteDeckPreparedStatement{ m_DatabaseConnection.Prepare(Infrastructure::Sql::DeleteDeckSql()) } {
    if (m_CreateRootDeckPreparedStatement->HasError()) {
        Support::ThrowError(m_CreateRootDeckPreparedStatement->GetError());
    }
    if (m_CreateChildDeckPreparedStatement->HasError()) {
        Support::ThrowError(m_CreateChildDeckPreparedStatement->GetError());
    }
    if (m_MoveDeckPreparedStatement->HasError()) {
        Support::ThrowError(m_MoveDeckPreparedStatement->GetError());
    }
    if (m_RenameDeckPreparedStatement->HasError()) {
        Support::ThrowError(m_RenameDeckPreparedStatement->GetError());
    }
    if (m_DeleteDeckCardReviewsPreparedStatement->HasError()) {
        Support::ThrowError(m_DeleteDeckCardReviewsPreparedStatement->GetError());
    }
    if (m_DeleteDeckCardsPreparedStatement->HasError()) {
        Support::ThrowError(m_DeleteDeckCardsPreparedStatement->GetError());
    }
    if (m_DeleteDeckPreparedStatement->HasError()) {
        Support::ThrowError(m_DeleteDeckPreparedStatement->GetError());
    }
}

DeckStore::~DeckStore() = default;

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateRootDeckPreparedStatement->Execute(DeckName.toStdString(), TargetLanguageCode) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateChildDeckPreparedStatement->Execute(ParentDeckId.toStdString(), DeckName.toStdString()) };
    std::optional<RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    ThrowOnMutationNoOp(QueryResult, "Child deck creation did not insert a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::MoveDeck(const QString& DeckId,
                                                                                             const std::optional<QString>& NewParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_MoveDeckPreparedStatement->Execute(
        DeckId.toStdString(), NewParentDeckId.has_value() ? duckdb::Value{ NewParentDeckId.value().toStdString() } : duckdb::Value{ nullptr }) };
    std::optional<RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    ThrowOnMutationNoOp(QueryResult, "Deck move did not update a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::RenameDeck(const QString& DeckId, const QString& NewDeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameDeckPreparedStatement->Execute(NewDeckName.toStdString(), DeckId.toStdString()) };
    std::optional<RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    ThrowOnMutationNoOp(QueryResult, "Deck rename did not update a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::DeleteDeck(const QString& DeckId) {
    m_DatabaseConnection.BeginTransaction();
    try {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteDeckCardReviewsPreparedStatement->Execute(DeckId.toStdString()) };
        ThrowOnQueryResultError(QueryResult);
        QueryResult = m_DeleteDeckCardsPreparedStatement->Execute(DeckId.toStdString());
        ThrowOnQueryResultError(QueryResult);
        QueryResult = m_DeleteDeckPreparedStatement->Execute(DeckId.toStdString());
        ThrowOnQueryResultError(QueryResult);
        ThrowOnMutationNoOp(QueryResult, "Deck delete did not delete a deck");
        m_DatabaseConnection.Commit();
        return std::nullopt;
    } catch (...) {
        m_DatabaseConnection.Rollback();
        throw;
    }
}

// TODO: Fix error message string checks and use switch case logic
[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum>
DeckStore::HandleRecoverableDeckMutationError(const std::unique_ptr<duckdb::QueryResult>& QueryResult) const {
    if (not QueryResult->HasError()) {
        return std::nullopt;
    }
    const std::string_view ErrorMessage{ QueryResult->GetError() };
    if (ErrorMessage.contains("deck_name_length_is_valid(\"name\")")) {
        return RecoverableDeckMutationErrorEnum::DeckNameLengthError;
    }
    if (ErrorMessage.contains("target_language_code_is_valid(target_language_code)")) {
        return RecoverableDeckMutationErrorEnum::InvalidTargetLanguageCodeError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Deck target language does not match parent deck")) {
        return RecoverableDeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Deck cannot move into itself") or
        ErrorMessage.starts_with("Invalid Input Error: Deck move would create a cycle") or
        ErrorMessage.starts_with("Invalid Input Error: deck move would create a cycle")) {
        return RecoverableDeckMutationErrorEnum::DeckTreeCycleDetectionError;
    }
    if (ErrorMessage.contains("Duplicate key \"")) {
        return RecoverableDeckMutationErrorEnum::DuplicateDeckNameError;
    }
    Support::ThrowError(QueryResult->GetError());
}
}
