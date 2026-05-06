#include "Infrastructure/Store/DeckStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Infrastructure/Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::CreateRootDeckSql(), DeckName.toStdString(), TargetLanguageCode) };
    return HandleDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) {
    std::optional<DeckMutationErrorEnum> DeckMutationError{ ValidateParentDeckExists(ParentDeckId) };
    if (DeckMutationError.has_value()) {
        return DeckMutationError;
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::CreateChildDeckSql(), ParentDeckId.toStdString(), DeckName.toStdString()) };
    return HandleDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::MoveDeck(const QString& DeckId, const std::optional<QString>& NewParentDeckId) {
    std::optional<DeckMutationErrorEnum> DeckMutationError{ ValidateDeckExists(DeckId) };
    if (DeckMutationError.has_value()) {
        return DeckMutationError;
    }
    if (NewParentDeckId.has_value()) {
        DeckMutationError = ValidateParentDeckExists(NewParentDeckId.value());
        if (DeckMutationError.has_value()) {
            return DeckMutationError;
        }
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::MoveDeckSql(),
        DeckId.toStdString(),
        NewParentDeckId.has_value() ? duckdb::Value{ NewParentDeckId.value().toStdString() } : duckdb::Value{ nullptr }) };
    return HandleDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::RenameDeck(const QString& DeckId, const QString& NewDeckName) {
    std::optional<DeckMutationErrorEnum> DeckMutationError{ ValidateDeckExists(DeckId) };
    if (DeckMutationError.has_value()) {
        return DeckMutationError;
    }
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Infrastructure::Sql::RenameDeckSql(), NewDeckName.toStdString(), DeckId.toStdString()) };
    return HandleDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::DeleteDeck(const QString& DeckId) {
    std::optional<DeckMutationErrorEnum> DeckMutationError{ ValidateDeckExists(DeckId) };
    if (DeckMutationError.has_value()) {
        return DeckMutationError;
    }
    m_DatabaseConnection.BeginTransaction();
    try {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Infrastructure::Sql::DeleteDeckCardReviewsSql(), DeckId.toStdString()) };
        DeckMutationError = HandleDeckMutationError(QueryResult);
        if (DeckMutationError.has_value()) {
            m_DatabaseConnection.Rollback();
            return DeckMutationError;
        }
        QueryResult = m_DatabaseConnection.Query(Infrastructure::Sql::DeleteDeckCardsSql(), DeckId.toStdString());
        DeckMutationError = HandleDeckMutationError(QueryResult);
        if (DeckMutationError.has_value()) {
            m_DatabaseConnection.Rollback();
            return DeckMutationError;
        }
        QueryResult = m_DatabaseConnection.Query(Infrastructure::Sql::DeleteDeckSql(), DeckId.toStdString());
        DeckMutationError = HandleDeckMutationError(QueryResult);
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

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::ValidateDeckExists(const QString& DeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Infrastructure::Sql::ValidateDeckExistsSql(), DeckId.toStdString()) };
    return HandleDeckMutationError(QueryResult);
}

[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum> DeckStore::ValidateParentDeckExists(const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Infrastructure::Sql::ValidateParentDeckExistsSql(),
                                                                                 ParentDeckId.toStdString()) };
    return HandleDeckMutationError(QueryResult);
}

// TODO: Fix error message string checks and use switch case logic
[[nodiscard]] std::optional<DeckStore::DeckMutationErrorEnum>
DeckStore::HandleDeckMutationError(const std::unique_ptr<duckdb::QueryResult>& QueryResult) const {
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
    if (ErrorMessage.starts_with("Invalid Input Error: Deck does not exist")) {
        return DeckMutationErrorEnum::DeckNotFoundError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Parent deck does not exist") or
        ErrorMessage.starts_with("Invalid Input Error: parent deck does not exist") or
        ErrorMessage.contains("Violates foreign key constraint because key \"id: ")) {
        return DeckMutationErrorEnum::ParentDeckNotFoundError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: Deck cannot move into itself") or
        ErrorMessage.starts_with("Invalid Input Error: Deck move would create a cycle") or
        ErrorMessage.starts_with("Invalid Input Error: deck move would create a cycle")) {
        return DeckMutationErrorEnum::DeckTreeCycleDetectionError;
    }
    if (ErrorMessage.contains("Duplicate key \"")) {
        return DeckMutationErrorEnum::DuplicateNameError;
    }
    Support::ThrowError(QueryResult->GetError());
}
}
