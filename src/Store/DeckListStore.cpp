#include "Store/DeckListStore.hpp"

#include <duckdb.hpp>

#include <string_view>

#include "Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Store {

QVector<DeckListStore::DeckListRowData> DeckListStore::ReadDeckList() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::ReadDeckListViewSql()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    QVector<DeckListRowData> DeckListRowDataVector;
    for (auto QueryResultIterator{ QueryResult->begin() };
         QueryResultIterator != QueryResult->end();
         ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        DeckListRowDataVector.emplace_back(
            QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
            QString{ QueryResultRow.GetValue<std::string>(1).c_str() },
            static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(2)),
            static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(3)),
            static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(4)));
    }
    return DeckListRowDataVector;
}

std::optional<std::int64_t> DeckListStore::ReadDeckListNextRefreshDelayMilliseconds() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::ReadDeckListNextRefreshDelayMillisecondsSql()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    auto QueryResultIterator{ QueryResult->begin() };
    if (QueryResultIterator != QueryResult->end()) {
        const auto& QueryResultRow{ *QueryResultIterator };
        const std::int64_t DeckListNextRefreshDelayMilliseconds{
            QueryResultRow.GetValue<std::int64_t>(0)
        };
        if (DeckListNextRefreshDelayMilliseconds < 0) {
            return std::nullopt;
        }
        return DeckListNextRefreshDelayMilliseconds;
    }
    return std::nullopt;
}

DeckListStore::DeckMutationStatus DeckListStore::CreateDeck(const QString& DeckName,
                                                            const quint8 TargetLanguageCode) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::CreateDeckSql(), DeckName.toStdString(), TargetLanguageCode) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

DeckListStore::DeckMutationStatus DeckListStore::UpdateDeckName(const QString& DeckId,
                                                                const QString& DeckName) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::UpdateDeckNameSql(), DeckName.toStdString(), DeckId.toStdString()) };
    return HandleRecoverableDeckMutationError(QueryResult);
}

void DeckListStore::DeleteDeck(const QString& DeckId) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
        Sql::DeleteDeckSql(), DeckId.toStdString()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
}

DeckListStore::DeckMutationStatus DeckListStore::HandleRecoverableDeckMutationError(
    const std::unique_ptr<duckdb::QueryResult>& QueryResult) const {
    if (!QueryResult->HasError()) {
        return DeckMutationStatus::Success;
    }
    if (QueryResult->GetErrorType() != duckdb::ExceptionType::CONSTRAINT) {
        Support::ThrowError(QueryResult->GetError());
    }
    const duckdb::ErrorData& ErrorData{ QueryResult->GetErrorObject() };
    const std::string_view RawMessage{ ErrorData.RawMessage() };
    if (RawMessage.contains("(deck_name_is_valid(\"name\"))")) {
        return DeckMutationStatus::NameLengthError;
    }
    if (RawMessage.contains("(target_language_code_is_valid(target_language_code))")) {
        return DeckMutationStatus::TargetLanguageCodeError;
    }
    if (RawMessage.starts_with("Duplicate key \"name:")) {
        return DeckMutationStatus::DuplicateNameError;
    }
    Support::ThrowError(QueryResult->GetError());
}
}
