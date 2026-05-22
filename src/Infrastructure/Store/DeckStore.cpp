#include "Infrastructure/Store/DeckStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Infrastructure/Sql/MutationSqlResource.hpp"
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

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateRootDeckPreparedStatement->Execute(DeckName.toStdString(), TargetLanguageCode) };
    return HandleDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_CreateChildDeckPreparedStatement->Execute(ParentDeckId.toStdString(), DeckName.toStdString()) };
    return HandleRequiredDeckMutation(QueryResult, "Child deck creation did not insert a deck");
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::MoveDeck(const QString& DeckId, const std::optional<QString>& NewParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_MoveDeckPreparedStatement->Execute(
        DeckId.toStdString(), NewParentDeckId.has_value() ? duckdb::Value{ NewParentDeckId.value().toStdString() } : duckdb::Value{ nullptr }) };
    return HandleRequiredDeckMutation(QueryResult, "Deck move did not update a deck");
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::RenameDeck(const QString& DeckId, const QString& NewDeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_RenameDeckPreparedStatement->Execute(NewDeckName.toStdString(), DeckId.toStdString()) };
    return HandleRequiredDeckMutation(QueryResult, "Deck rename did not update a deck");
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::DeleteDeck(const QString& DeckId) {
    m_DatabaseConnection.BeginTransaction();
    try {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DeleteDeckCardReviewsPreparedStatement->Execute(DeckId.toStdString()) };
        std::optional<DeckMutationErrorEnum> DeckMutationError{ HandleDeckMutationError(QueryResult) };
        if (DeckMutationError.has_value()) {
            m_DatabaseConnection.Rollback();
            return DeckMutationError;
        }
        QueryResult = m_DeleteDeckCardsPreparedStatement->Execute(DeckId.toStdString());
        DeckMutationError = HandleDeckMutationError(QueryResult);
        if (DeckMutationError.has_value()) {
            m_DatabaseConnection.Rollback();
            return DeckMutationError;
        }
        QueryResult = m_DeleteDeckPreparedStatement->Execute(DeckId.toStdString());
        DeckMutationError = HandleRequiredDeckMutation(QueryResult, "Deck delete did not delete a deck");
        if (DeckMutationError.has_value()) {
            m_DatabaseConnection.Rollback();
            return DeckMutationError;
        }
        m_DatabaseConnection.Commit();
        return std::nullopt;
    } catch (...) {
        m_DatabaseConnection.Rollback();
        throw;
    }
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::HandleRequiredDeckMutation(const std::unique_ptr<duckdb::QueryResult>& QueryResult,
                                                                                                    const std::string_view EmptyResultMessage) const {
    std::optional<DeckMutationErrorEnum> DeckMutationError{ HandleDeckMutationError(QueryResult) };
    if (DeckMutationError.has_value()) {
        return DeckMutationError;
    }
    if (QueryResult->begin() not_eq QueryResult->end()) {
        return std::nullopt;
    }
    Support::ThrowError(EmptyResultMessage);
}

// TODO: Fix error message string checks and use switch case logic
[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum>
DeckStore::HandleDeckMutationError(const std::unique_ptr<duckdb::QueryResult>& QueryResult) const {
    if (not QueryResult->HasError()) {
        return std::nullopt;
    }
    const std::string_view ErrorMessage{ QueryResult->GetError() };
    if (ErrorMessage.contains("deck_name_length_is_valid(\"name\")")) {
        return DeckMutationErrorEnum::DeckNameLengthError;
    }
    if (ErrorMessage.contains("target_language_code_is_valid(target_language_code)")) {
        return DeckMutationErrorEnum::InvalidTargetLanguageCodeError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Deck target language does not match parent deck")) {
        return DeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Deck cannot move into itself") or
        ErrorMessage.starts_with("Invalid Input Error: Deck move would create a cycle") or
        ErrorMessage.starts_with("Invalid Input Error: deck move would create a cycle")) {
        return DeckMutationErrorEnum::DeckTreeCycleDetectionError;
    }
    if (ErrorMessage.contains("Duplicate key \"")) {
        return DeckMutationErrorEnum::DuplicateDeckNameError;
    }
    Support::ThrowError(QueryResult->GetError());
}
}
