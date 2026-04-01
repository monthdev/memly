#include "Bridge/DeckListBridge.hpp"

#include <duckdb.hpp>

#include "Model/DeckListModel.hpp"
#include "Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Bridge {

[[nodiscard]] Q_INVOKABLE QString DeckListBridge::NameLengthErrorMessage() {
    return QString{ "Deck name length exceeds character limit" };
}

[[nodiscard]] Q_INVOKABLE QString DeckListBridge::DuplicateNameErrorMessage() {
    return QString{ "Deck name already exists" };
}

[[nodiscard]] Q_INVOKABLE QString DeckListBridge::TargetLanguageCodeErrorMessage() {
    return QString{ "Target language code is invalid" };
}

[[nodiscard]] QString DeckListBridge::HandleQueryResultRecoverableError(
    const std::unique_ptr<duckdb::QueryResult>& QueryResult) {
    switch (QueryResult->GetErrorType()) {
    case duckdb::ExceptionType::INVALID: {
        return QString{};
    }
    case duckdb::ExceptionType::CONSTRAINT: {
        const duckdb::ErrorData& ErrorData{ QueryResult->GetErrorObject() };
        std::string_view RawMessage{ ErrorData.RawMessage() };
        if (RawMessage.contains("(deck_name_is_valid(\"name\"))")) {
            return NameLengthErrorMessage();
        }
        if (RawMessage.contains("(target_language_code_is_valid(target_language_code))")) {
            return TargetLanguageCodeErrorMessage();
        }
        if (RawMessage.starts_with("Duplicate key \"name:")) {
            return DuplicateNameErrorMessage();
        }
        Support::ThrowError(QueryResult->GetError());
    }
    default: {
        Support::ThrowError(QueryResult->GetError());
    }
    }
}

[[nodiscard]] Q_INVOKABLE QString DeckListBridge::CreateDeck(const QString& DeckName,
                                                             const quint8 TargetLanguageCode) {
    return Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
            Sql::CreateDeckSql(), DeckName.toStdString(), TargetLanguageCode) };
        const QString Error{ HandleQueryResultRecoverableError(QueryResult) };
        if (Error.isEmpty()) {
            RefreshDeckList();
        }
        return Error;
    });
}

[[nodiscard]] Q_INVOKABLE QString DeckListBridge::UpdateDeckName(const QString& DeckId,
                                                                 const QString& DeckName) {
    return Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
            Sql::UpdateDeckNameSql(), DeckName.toStdString(), DeckId.toStdString()) };
        const QString Error{ HandleQueryResultRecoverableError(QueryResult) };
        if (Error.isEmpty()) {
            RefreshDeckList();
        }
        return Error;
    });
}

Q_INVOKABLE void DeckListBridge::DeleteDeck(const QString& DeckId) {
    Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
            Sql::DeleteDeckSql(), DeckId.toStdString()) };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        RefreshDeckList();
    });
}

void DeckListBridge::RefreshDeckList() {
    Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(
            Sql::ReadDeckListViewSql()) };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        QVector<Model::DeckListModel::DeckListRow> DeckList;
        for (auto QueryResultIterator{ QueryResult->begin() };
             QueryResultIterator != QueryResult->end();
             ++QueryResultIterator) {
            const auto& QueryResultRow{ *QueryResultIterator };
            DeckList.emplace_back(QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
                                  QString{ QueryResultRow.GetValue<std::string>(1).c_str() });
        }
        m_DeckList.ReplaceAll(std::move(DeckList));
    });
}
}
