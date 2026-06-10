#pragma once

#include <duckdb.hpp>

#include <QString>
#include <QtGlobal>
#include <memory>
#include <optional>

#include "Infrastructure/Sql/Deck/Mutation/DeckMutationSql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Deck {

class DeckStore final {
public:
    enum class RecoverableDeckMutationErrorEnum : std::uint8_t {
        DeckNameLengthError,
        DuplicateSiblingDeckNameError,
        InvalidTargetLanguageCodeError,
        ParentDeckTargetLanguageMismatchError,
        DeckTreeCycleDetectionError
    };

    explicit DeckStore(duckdb::Connection& DatabaseConnection)
        : m_CreateRootDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::CreateRootDeckSql()) }
        , m_CreateChildDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::CreateChildDeckSql()) }
        , m_MoveDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::MoveDeckSql()) }
        , m_RenameDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::RenameDeckSql()) }
        , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckCardReviewsSql()) }
        , m_DeleteDeckCardsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckCardsSql()) }
        , m_DeleteDeckPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Mutation::DeleteDeckSql()) } {
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateRootDeckPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_CreateChildDeckPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_MoveDeckPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_RenameDeckPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckCardReviewsPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckCardsPreparedStatement);
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_DeleteDeckPreparedStatement);
    }

    ~DeckStore() = default;
    DeckStore(const DeckStore&) = delete;
    DeckStore(DeckStore&&) = delete;
    DeckStore& operator=(const DeckStore&) = delete;
    DeckStore& operator=(DeckStore&&) = delete;

    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> CreateRootDeck(const QString&, quint8);
    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> CreateChildDeck(const QString&, const QString&);
    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> MoveDeck(const QString&, const std::optional<QString>&);
    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> RenameDeck(const QString&, const QString&);
    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> DeleteDeck(const QString&);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_CreateRootDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateChildDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_MoveDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_RenameDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardReviewsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckPreparedStatement;

    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> HandleRecoverableDeckMutationError(duckdb::QueryResult&) const;
};

}
