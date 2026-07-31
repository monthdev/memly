#include "Layer/Infrastructure/Store/Deck/DeckStore.hpp"

#include <duckdb.hpp>

#include <cassert>
#include <cstdint>
#include <memory>
#include <string>

#include "Layer/Infrastructure/Database/DatabaseRuntime.hpp"

namespace Layer::Infrastructure::Store::Deck {

void DeckStore::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) {
    [[maybe_unused]] const std::unique_ptr<duckdb::QueryResult> QueryResult{
        m_DatabaseRuntime.ExecutePreparedStatement(m_CreateRootDeckPreparedStatement).WithParameters(DeckName, TargetLanguageCode)
    };
    assert(QueryResult->type == duckdb::QueryResultType::MATERIALIZED_RESULT and QueryResult->Cast<duckdb::MaterializedQueryResult>().RowCount() == 1);
}

void DeckStore::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) {
    [[maybe_unused]] const std::unique_ptr<duckdb::QueryResult> QueryResult{
        m_DatabaseRuntime.ExecutePreparedStatement(m_CreateChildDeckPreparedStatement).WithParameters(ParentDeckId, DeckName)
    };
    assert(QueryResult->type == duckdb::QueryResultType::MATERIALIZED_RESULT and QueryResult->Cast<duckdb::MaterializedQueryResult>().RowCount() == 1);
}

void DeckStore::MoveDeckToRoot(const std::string& DeckId) {
    [[maybe_unused]] const std::unique_ptr<duckdb::QueryResult> QueryResult{
        m_DatabaseRuntime.ExecutePreparedStatement(m_MoveDeckToRootPreparedStatement).WithParameters(DeckId)
    };
    assert(QueryResult->type == duckdb::QueryResultType::MATERIALIZED_RESULT and QueryResult->Cast<duckdb::MaterializedQueryResult>().RowCount() == 1);
}

void DeckStore::MoveDeckUnderParent(const std::string& DeckId, const std::string& ParentDeckId) {
    [[maybe_unused]] const std::unique_ptr<duckdb::QueryResult> QueryResult{
        m_DatabaseRuntime.ExecutePreparedStatement(m_MoveDeckUnderParentPreparedStatement).WithParameters(DeckId, ParentDeckId)
    };
    assert(QueryResult->type == duckdb::QueryResultType::MATERIALIZED_RESULT and QueryResult->Cast<duckdb::MaterializedQueryResult>().RowCount() == 1);
}

void DeckStore::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) {
    [[maybe_unused]] const std::unique_ptr<duckdb::QueryResult> QueryResult{
        m_DatabaseRuntime.ExecutePreparedStatement(m_RenameDeckPreparedStatement).WithParameters(NewDeckName, DeckId)
    };
    assert(QueryResult->type == duckdb::QueryResultType::MATERIALIZED_RESULT and QueryResult->Cast<duckdb::MaterializedQueryResult>().RowCount() == 1);
}

void DeckStore::DeleteDeck(const std::string& DeckId) {
    static_cast<void>(m_DatabaseRuntime.ExecutePreparedStatement(m_DeleteDeckCardReviewsPreparedStatement).WithParameters(DeckId));
    static_cast<void>(m_DatabaseRuntime.ExecutePreparedStatement(m_DeleteDeckCardsPreparedStatement).WithParameters(DeckId));
    [[maybe_unused]] const std::unique_ptr<duckdb::QueryResult> QueryResult{
        m_DatabaseRuntime.ExecutePreparedStatement(m_DeleteDeckPreparedStatement).WithParameters(DeckId)
    };
    assert(QueryResult->type == duckdb::QueryResultType::MATERIALIZED_RESULT and QueryResult->Cast<duckdb::MaterializedQueryResult>().RowCount() >= 1);
}

}
