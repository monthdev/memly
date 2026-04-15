#include "Store/DeckHierarchyStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Store {

QVector<DeckHierarchyStore::DeckHierarchyRow> DeckHierarchyStore::ReadDeckHierarchyView() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::ReadDeckHierarchyViewSql()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    QVector<DeckHierarchyRow> DeckHierarchyRowVector;
    for (auto QueryResultIterator{ QueryResult->begin() };
         QueryResultIterator != QueryResult->end();
         ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        DeckHierarchyRowVector.emplace_back(
            QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
            QString{ QueryResultRow.GetValue<std::string>(1).c_str() },
            QString{ QueryResultRow.GetValue<std::string>(2).c_str() },
            static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(3)),
            static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(4)),
            static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(5)));
    }
    return DeckHierarchyRowVector;
}

std::optional<std::int64_t>
DeckHierarchyStore::ReadDeckHierarchyViewNextRefreshDelayMilliseconds() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::ReadDeckHierarchyViewNextRefreshDelayMillisecondsSql()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    auto QueryResultIterator{ QueryResult->begin() };
    if (QueryResultIterator != QueryResult->end()) {
        // NOTE: Empty QueryResultIterator guard above is technically unnecessary
        const auto& QueryResultRow{ *QueryResultIterator };
        const std::int64_t DeckHierarchyViewNextRefreshDelayMilliseconds{
            QueryResultRow.GetValue<std::int64_t>(0)
        };
        if (DeckHierarchyViewNextRefreshDelayMilliseconds < 0) {
            return std::nullopt;
        }
        return DeckHierarchyViewNextRefreshDelayMilliseconds;
    }
    return std::nullopt;
}

DeckHierarchyStore::DeckMutationStatus
DeckHierarchyStore::CreateDeck(const QString& DeckName,
                               const quint8 TargetLanguageCode,
                               const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::CreateDeckSql(),
        ParentDeckId.toStdString(),
        DeckName.toStdString(),
        TargetLanguageCode) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

DeckHierarchyStore::DeckMutationStatus DeckHierarchyStore::MoveDeck(const QString& DeckId,
                                                                    const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::MoveDeckSql(), DeckId.toStdString(), ParentDeckId.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

DeckHierarchyStore::DeckMutationStatus DeckHierarchyStore::UpdateDeckName(const QString& DeckId,
                                                                          const QString& DeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::UpdateDeckNameSql(), DeckName.toStdString(), DeckId.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

void DeckHierarchyStore::DeleteDeck(const QString& DeckId) {
    m_DatabaseConnection.BeginTransaction();
    try {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
            Sql::DeleteDeckCardReviewsSql(), DeckId.toStdString()) };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        QueryResult = m_DatabaseConnection.Query(Sql::DeleteDeckCardsSql(), DeckId.toStdString());
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        QueryResult = m_DatabaseConnection.Query(Sql::DeleteDeckSql(), DeckId.toStdString());
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
DeckHierarchyStore::DeckMutationStatus DeckHierarchyStore::HandleRecoverableDeckMutationError(
    const std::unique_ptr<duckdb::QueryResult>& QueryResult) const {
    if (!QueryResult->HasError()) {
        return DeckMutationStatus::Success;
    }
    const std::string_view ErrorMessage{ QueryResult->GetError() };
    if (ErrorMessage.contains("deck_name_length_is_valid(\"name\")")) {
        return DeckMutationStatus::NameLengthError;
    }
    if (ErrorMessage.contains("target_language_code_is_valid(target_language_code)")) {
        return DeckMutationStatus::TargetLanguageCodeError;
    }
    if (ErrorMessage.starts_with("Invalid Input Error: parent deck does not exist") ||
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
