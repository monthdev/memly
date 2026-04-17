#include "Store/DeckHierarchyStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Store {

DeckHierarchyStore::DeckHierarchyViewSnapshot DeckHierarchyStore::ReadDeckHierarchyViewSnapshot() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Sql::ReadDeckHierarchyViewSnapshotSql()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    DeckHierarchyViewSnapshot DeckHierarchyViewSnapshot{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        DeckHierarchyViewSnapshot.m_DeckHierarchyRowQVector.emplace_back(QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
                                                                         QString{ QueryResultRow.GetValue<std::string>(1).c_str() },
                                                                         QString{ QueryResultRow.GetValue<std::string>(2).c_str() },
                                                                         static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(3)),
                                                                         static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(4)),
                                                                         static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(5)),
                                                                         static_cast<quint8>(QueryResultRow.GetValue<std::uint8_t>(6)));
        if (not DeckHierarchyViewSnapshot.m_NextRefreshAtMillisecondsSinceEpoch.has_value()) {
            std::int64_t NextRefreshAtMillisecondsSinceEpoch = QueryResultRow.GetValue<std::int64_t>(7);
            if (NextRefreshAtMillisecondsSinceEpoch > 0) {
                DeckHierarchyViewSnapshot.m_NextRefreshAtMillisecondsSinceEpoch = NextRefreshAtMillisecondsSinceEpoch;
            }
        }
    }
    return DeckHierarchyViewSnapshot;
}

DeckHierarchyStore::DeckMutationStatus DeckHierarchyStore::CreateDeck(const QString& DeckName, const quint8 TargetLanguageCode, const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::CreateDeckSql(), ParentDeckId.toStdString(), DeckName.toStdString(), TargetLanguageCode) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

DeckHierarchyStore::DeckMutationStatus DeckHierarchyStore::MoveDeck(const QString& DeckId, const QString& ParentDeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Sql::MoveDeckSql(), DeckId.toStdString(), ParentDeckId.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

DeckHierarchyStore::DeckMutationStatus DeckHierarchyStore::UpdateDeckName(const QString& DeckId, const QString& DeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Sql::UpdateDeckNameSql(), DeckName.toStdString(), DeckId.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

void DeckHierarchyStore::DeleteDeck(const QString& DeckId) {
    m_DatabaseConnection.BeginTransaction();
    try {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Sql::DeleteDeckCardReviewsSql(), DeckId.toStdString()) };
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
DeckHierarchyStore::DeckMutationStatus DeckHierarchyStore::HandleRecoverableDeckMutationError(const std::unique_ptr<duckdb::QueryResult>& QueryResult) const {
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
