#include "Infrastructure/Store/Deck/DeckStore.hpp"

#include <duckdb.hpp>

#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Deck {

void DeckStore::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateRootDeckPreparedStatement->Execute(DeckName, TargetLanguageCode) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    assert(QueryResult->begin() not_eq QueryResult->end());
}

void DeckStore::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateChildDeckPreparedStatement->Execute(ParentDeckId, DeckName) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    assert(QueryResult->begin() not_eq QueryResult->end());
}

void DeckStore::MoveDeck(const std::string& DeckId, const std::optional<std::string>& NewParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_MoveDeckPreparedStatement->Execute(
        DeckId, NewParentDeckId.has_value() ? duckdb::Value{ NewParentDeckId.value() } : duckdb::Value{ nullptr }) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    assert(QueryResult->begin() not_eq QueryResult->end());
}

void DeckStore::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameDeckPreparedStatement->Execute(NewDeckName, DeckId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    assert(QueryResult->begin() not_eq QueryResult->end());
}

void DeckStore::DeleteDeck(const std::string& DeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteDeckCardReviewsPreparedStatement->Execute(DeckId) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    QueryResult = m_DeleteDeckCardsPreparedStatement->Execute(DeckId);
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    QueryResult = m_DeleteDeckPreparedStatement->Execute(DeckId);
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    assert(QueryResult->begin() not_eq QueryResult->end());
}

}
