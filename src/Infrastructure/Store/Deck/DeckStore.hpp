#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <string>

#include "Domain/Deck/RecoverableDeckIdError.hpp"
#include "Domain/Deck/RecoverableDeckMutationError.hpp"
#include "Infrastructure/Sql/Deck/Mutation/DeckMutationSql.hpp"
#include "Infrastructure/Sql/Deck/Query/DeckQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Deck {

class DeckStore final {
public:
    explicit DeckStore(duckdb::Connection& DatabaseConnection)
        : m_CheckDeckIdExistsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Query::CheckDeckIdExistsSql()) }
        , m_CreateRootDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::CreateRootDeckSql()) }
        , m_CreateChildDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::CreateChildDeckSql()) }
        , m_MoveDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::MoveDeckSql()) }
        , m_RenameDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::RenameDeckSql()) }
        , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckCardReviewsSql()) }
        , m_DeleteDeckCardsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckCardsSql()) }
        , m_DeleteDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckSql()) } {
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CheckDeckIdExistsPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateRootDeckPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateChildDeckPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_MoveDeckPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_RenameDeckPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckCardReviewsPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckCardsPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckPreparedStatement);
    }

    ~DeckStore() noexcept = default;
    DeckStore(const DeckStore&) = delete;
    DeckStore(DeckStore&&) = delete;
    DeckStore& operator=(const DeckStore&) = delete;
    DeckStore& operator=(DeckStore&&) = delete;

    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateRootDeck(const std::string&, std::uint8_t);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> CreateChildDeck(const std::string&, const std::string&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> MoveDeck(const std::string&, const std::optional<std::string>&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckMutationErrorEnum> RenameDeck(const std::string&, const std::string&);
    [[nodiscard]] std::expected<void, Domain::Deck::RecoverableDeckIdErrorEnum> DeleteDeck(const std::string&);

private:
    [[nodiscard]] bool CheckDeckIdExists(const std::string&);

    std::unique_ptr<duckdb::PreparedStatement> m_CheckDeckIdExistsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateRootDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateChildDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_MoveDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_RenameDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardReviewsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckPreparedStatement;
};

}
