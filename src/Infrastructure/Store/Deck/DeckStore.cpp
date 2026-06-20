#include "Infrastructure/Store/Deck/DeckStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Runtime/Crash.hpp"

namespace Infrastructure::Store::Deck {
namespace {

// TODO: Fix error message string checks and use switch case logic
[[nodiscard]] std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> u_TryHandleRecoverableDeckMutationError(duckdb::QueryResult& QueryResult) {
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

[[nodiscard]] bool DeckStore::CheckDeckIdExists(const std::string& DeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CheckDeckIdExistsPreparedStatement->Execute(DeckId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    return QueryResult->begin() not_eq QueryResult->end();
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::CreateRootDeck(const std::string& DeckName,
                                                                                                            const std::uint8_t TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateRootDeckPreparedStatement->Execute(DeckName, TargetLanguageCode) };
    const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationErrorOptional{ u_TryHandleRecoverableDeckMutationError(
        *QueryResult) };
    if (RecoverableDeckMutationErrorOptional.has_value()) {
        return std::unexpected{ RecoverableDeckMutationErrorOptional.value() };
    }
    return {};
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::CreateChildDeck(const std::string& DeckName,
                                                                                                             const std::string& ParentDeckId) {
    if (not CheckDeckIdExists(ParentDeckId)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidParentDeckIdError };
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateChildDeckPreparedStatement->Execute(ParentDeckId, DeckName) };
    const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationErrorOptional{ u_TryHandleRecoverableDeckMutationError(
        *QueryResult) };
    if (RecoverableDeckMutationErrorOptional.has_value()) {
        return std::unexpected{ RecoverableDeckMutationErrorOptional.value() };
    }
    return {};
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::MoveDeck(const std::string& DeckId,
                                                                                                      const std::optional<std::string>& NewParentDeckId) {
    if (not CheckDeckIdExists(DeckId)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidDeckIdError };
    }
    if (NewParentDeckId.has_value()) {
        if (not CheckDeckIdExists(NewParentDeckId.value())) {
            return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidParentDeckIdError };
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_MoveDeckPreparedStatement->Execute(
        DeckId, NewParentDeckId.has_value() ? duckdb::Value{ NewParentDeckId.value() } : duckdb::Value{ nullptr }) };
    const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationErrorOptional{ u_TryHandleRecoverableDeckMutationError(
        *QueryResult) };
    if (RecoverableDeckMutationErrorOptional.has_value()) {
        return std::unexpected{ RecoverableDeckMutationErrorOptional.value() };
    }
    return {};
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> DeckStore::RenameDeck(const std::string& DeckId,
                                                                                                        const std::string& NewDeckName) {
    if (not CheckDeckIdExists(DeckId)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckMutationErrorEnum::InvalidDeckIdError };
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameDeckPreparedStatement->Execute(NewDeckName, DeckId) };
    const std::optional<Domain::Deck::RecoverableDeckMutationErrorEnum> RecoverableDeckMutationErrorOptional{ u_TryHandleRecoverableDeckMutationError(
        *QueryResult) };
    if (RecoverableDeckMutationErrorOptional.has_value()) {
        return std::unexpected{ RecoverableDeckMutationErrorOptional.value() };
    }
    return {};
}

[[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckIdErrorEnum> DeckStore::DeleteDeck(const std::string& DeckId) {
    if (not CheckDeckIdExists(DeckId)) {
        return std::unexpected{ Domain::Deck::RecoverableDeckIdErrorEnum::InvalidDeckIdError };
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteDeckCardReviewsPreparedStatement->Execute(DeckId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    QueryResult = m_DeleteDeckCardsPreparedStatement->Execute(DeckId);
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    QueryResult = m_DeleteDeckPreparedStatement->Execute(DeckId);
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    return {};
}

}
