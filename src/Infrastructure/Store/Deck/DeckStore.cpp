#include "Infrastructure/Store/Deck/DeckStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Infrastructure/Sql/Mutation/Deck/DeckMutationSql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"
#include "Runtime/Crash.hpp"

namespace Infrastructure::Store::Deck {

DeckStore::DeckStore(duckdb::Connection& DatabaseConnection)
    : m_CreateRootDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::Deck::CreateRootDeckSql()) }
    , m_CreateChildDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::Deck::CreateChildDeckSql()) }
    , m_MoveDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::Deck::MoveDeckSql()) }
    , m_RenameDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::Deck::RenameDeckSql()) }
    , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::Deck::DeleteDeckCardReviewsSql()) }
    , m_DeleteDeckCardsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::Deck::DeleteDeckCardsSql()) }
    , m_DeleteDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Mutation::Deck::DeleteDeckSql()) } {
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateRootDeckPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateChildDeckPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_MoveDeckPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_RenameDeckPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckCardReviewsPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckCardsPreparedStatement);
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckPreparedStatement);
}

DeckStore::~DeckStore() = default;

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateRootDeckPreparedStatement->Execute(DeckName.toStdString(), TargetLanguageCode) };
    return HandleRecoverableDeckMutationError(*QueryResult);
}

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateChildDeckPreparedStatement->Execute(ParentDeckId.toStdString(), DeckName.toStdString()) };
    std::optional<RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(*QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Child deck creation did not insert a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::MoveDeck(const QString& DeckId,
                                                                                             const std::optional<QString>& NewParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_MoveDeckPreparedStatement->Execute(
        DeckId.toStdString(), NewParentDeckId.has_value() ? duckdb::Value{ NewParentDeckId.value().toStdString() } : duckdb::Value{ nullptr }) };
    std::optional<RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(*QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Deck move did not update a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::RenameDeck(const QString& DeckId, const QString& NewDeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameDeckPreparedStatement->Execute(NewDeckName.toStdString(), DeckId.toStdString()) };
    std::optional<RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(*QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Deck rename did not update a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::DeleteDeck(const QString& DeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteDeckCardReviewsPreparedStatement->Execute(DeckId.toStdString()) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    QueryResult = m_DeleteDeckCardsPreparedStatement->Execute(DeckId.toStdString());
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    QueryResult = m_DeleteDeckPreparedStatement->Execute(DeckId.toStdString());
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Deck delete did not delete a deck");
    return std::nullopt;
}

// TODO: Fix error message string checks and use switch case logic
[[nodiscard]] std::optional<DeckStore::RecoverableDeckMutationErrorEnum> DeckStore::HandleRecoverableDeckMutationError(duckdb::QueryResult& QueryResult) const {
    if (not QueryResult.HasError()) {
        return std::nullopt;
    }
    const std::string_view ErrorMessage{ QueryResult.GetError() };
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
        return RecoverableDeckMutationErrorEnum::DuplicateSiblingDeckNameError;
    }
    Runtime::ThrowError(QueryResult.GetError());
}
}
