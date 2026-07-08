#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/Deck/Mutation/DeckMutationSql.hpp"

namespace Infrastructure::Store::Deck {

class DeckStore final {
public:
    explicit DeckStore(duckdb::Connection& DatabaseConnection)
        : m_CreateRootDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::CreateRootDeckSql()) }
        , m_CreateChildDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::CreateChildDeckSql()) }
        , m_MoveDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::MoveDeckSql()) }
        , m_RenameDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::RenameDeckSql()) }
        , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckCardReviewsSql()) }
        , m_DeleteDeckCardsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckCardsSql()) }
        , m_DeleteDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_CreateRootDeckPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_CreateChildDeckPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_MoveDeckPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_RenameDeckPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_DeleteDeckCardReviewsPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_DeleteDeckCardsPreparedStatement);
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_DeleteDeckPreparedStatement);
    }
    DeckStore(const DeckStore&) = delete;
    DeckStore(DeckStore&&) = delete;
    DeckStore& operator=(const DeckStore&) = delete;
    DeckStore& operator=(DeckStore&&) = delete;

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeck(const std::string&, const std::optional<std::string>&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_CreateRootDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateChildDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_MoveDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_RenameDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardReviewsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckPreparedStatement;
};

}
