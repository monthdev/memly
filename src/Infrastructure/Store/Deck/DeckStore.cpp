#include "Infrastructure/Store/Deck/DeckStore.hpp"

#include <duckdb.hpp>

#include <memory>
#include <string_view>

#include "Infrastructure/Sql/SqlExecutionGuard.hpp"
#include "Runtime/Crash.hpp"

namespace Infrastructure::Store::Deck {

[[nodiscard]] std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::CreateRootDeck(const QString& DeckName,
                                                                                                      const quint8 TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateRootDeckPreparedStatement->Execute(DeckName.toStdString(), TargetLanguageCode) };
    return HandleRecoverableDeckMutationError(*QueryResult);
}

[[nodiscard]] std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateChildDeckPreparedStatement->Execute(ParentDeckId.toStdString(), DeckName.toStdString()) };
    std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(*QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Child deck creation did not insert a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::MoveDeck(const QString& DeckId,
                                                                                                const std::optional<QString>& NewParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_MoveDeckPreparedStatement->Execute(
        DeckId.toStdString(), NewParentDeckId.has_value() ? duckdb::Value{ NewParentDeckId.value().toStdString() } : duckdb::Value{ nullptr }) };
    std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(*QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Deck move did not update a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::RenameDeck(const QString& DeckId, const QString& NewDeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameDeckPreparedStatement->Execute(NewDeckName.toStdString(), DeckId.toStdString()) };
    std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationError{ HandleRecoverableDeckMutationError(*QueryResult) };
    if (RecoverableDeckMutationError.has_value()) {
        return RecoverableDeckMutationError;
    }
    Infrastructure::Sql::ThrowOnMutationNoOp(*QueryResult, "Deck rename did not update a deck");
    return std::nullopt;
}

[[nodiscard]] std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::DeleteDeck(const QString& DeckId) {
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
[[nodiscard]] std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum>
DeckStore::HandleRecoverableDeckMutationError(duckdb::QueryResult& QueryResult) const {
    if (not QueryResult.HasError()) {
        return std::nullopt;
    }
    const std::string_view ErrorMessage{ QueryResult.GetError() };
    if (ErrorMessage.contains("deck_name_length_is_valid(\"name\")")) {
        return Domain::Deck::RecoverableDeckMutationErrorEnum::DeckNameLengthError;
    }
    if (ErrorMessage.contains("target_language_code_is_valid(target_language_code)")) {
        return Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidTargetLanguageCodeError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Deck target language does not match parent deck")) {
        return Domain::Deck::RecoverableDeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Deck cannot move into itself") or
        ErrorMessage.starts_with("Invalid Input Error: Deck move would create a cycle") or
        ErrorMessage.starts_with("Invalid Input Error: deck move would create a cycle")) {
        return Domain::Deck::RecoverableDeckMutationErrorEnum::DeckTreeCycleDetectionError;
    }
    if (ErrorMessage.contains("Duplicate key \"")) {
        return Domain::Deck::RecoverableDeckMutationErrorEnum::DuplicateSiblingDeckNameError;
    }
    Runtime::ThrowError(QueryResult.GetError());
}
}
