#include "Infrastructure/Store/DeckStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Infrastructure/Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

DeckStore::DeckMutationStatus DeckStore::CreateDeck(const QString& DeckName, const quint8 TargetLanguageCode, const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::CreateDeckSql(), ParentDeckId.toStdString(), DeckName.toStdString(), TargetLanguageCode) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

DeckStore::DeckMutationStatus DeckStore::MoveDeck(const QString& DeckId, const QString& NewParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::MoveDeckSql(), DeckId.toStdString(), NewParentDeckId.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

DeckStore::DeckMutationStatus DeckStore::UpdateDeckName(const QString& DeckId, const QString& DeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::UpdateDeckNameSql(), DeckName.toStdString(), DeckId.toStdString()) };
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
DeckStore::DeckMutationStatus DeckStore::HandleRecoverableDeckMutationError(const std::unique_ptr<duckdb::QueryResult>& QueryResult) const {
    if (not QueryResult->HasError()) {
        return DeckMutationStatus::Success;
    }
    const std::string_view ErrorMessage{ QueryResult->GetError() };
    if (ErrorMessage.contains("deck_name_length_is_valid(\"name\")")) {
        return DeckMutationStatus::NameLengthError;
    }
    if (ErrorMessage.contains("target_language_code_is_valid(target_language_code)")) {
        return DeckMutationStatus::TargetLanguageCodeError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: parent deck does not exist") or
        ErrorMessage.contains("Violates foreign key constraint because key \"id: ")) {
        return DeckMutationStatus::ParentDeckError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: deck move would create a cycle")) {
        return DeckMutationStatus::CycleDetectionError;
    }
    if (ErrorMessage.contains("Duplicate key \"")) {
        return DeckMutationStatus::DuplicateNameError;
    }
    Support::ThrowError(QueryResult->GetError());
}

}
