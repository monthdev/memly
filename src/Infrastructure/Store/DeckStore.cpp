#include "Infrastructure/Store/DeckStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Infrastructure/Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::CreateRootDeckSql(), DeckName.toStdString(), TargetLanguageCode) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) {
    if (ParentDeckId.isEmpty()) {
        return DeckMutationErrorEnum::ParentDeckError;
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::CreateChildDeckSql(), ParentDeckId.toStdString(), DeckName.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::MoveDeck(const QString& DeckId, const QString& NewParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::MoveDeckSql(), DeckId.toStdString(), NewParentDeckId.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::UpdateDeckName(const QString& DeckId, const QString& NewDeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::UpdateDeckNameSql(), NewDeckName.toStdString(), DeckId.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

void DeckStore::DeleteDeck(const QString& DeckId) {
    m_DatabaseConnection.BeginTransaction();
    try {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Infrastructure::Sql::DeleteDeckCardReviewsSql(), DeckId.toStdString()) };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        QueryResult = m_DatabaseConnection.Query(Infrastructure::Sql::DeleteDeckCardsSql(), DeckId.toStdString());
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        QueryResult = m_DatabaseConnection.Query(Infrastructure::Sql::DeleteDeckSql(), DeckId.toStdString());
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        m_DatabaseConnection.Commit();
    } catch (...) {
        m_DatabaseConnection.Rollback();
        throw;
    }
}

// TODO: Fix error message string checks and use switch case logic
[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum>
DeckStore::HandleRecoverableDeckMutationError(const std::unique_ptr<duckdb::QueryResult>& QueryResult) const {
    if (not QueryResult->HasError()) {
        return std::nullopt;
    }
    const std::string_view ErrorMessage{ QueryResult->GetError() };
    if (ErrorMessage.contains("deck_name_length_is_valid(\"name\")")) {
        return DeckMutationErrorEnum::NameLengthError;
    }
    if (ErrorMessage.contains("target_language_code_is_valid(target_language_code)")) {
        return DeckMutationErrorEnum::TargetLanguageCodeError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Deck target language does not match parent deck")) {
        return DeckMutationErrorEnum::ParentDeckTargetLanguageMismatchError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: parent deck does not exist") or
        ErrorMessage.contains("Violates foreign key constraint because key \"id: ")) {
        return DeckMutationErrorEnum::ParentDeckError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: deck move would create a cycle")) {
        return DeckMutationErrorEnum::CycleDetectionError;
    }
    if (ErrorMessage.contains("Duplicate key \"")) {
        return DeckMutationErrorEnum::DuplicateNameError;
    }
    Support::ThrowError(QueryResult->GetError());
}
}
