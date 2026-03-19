#include "Bridge/DeckBridge.hpp"

#include <duckdb.hpp>

#include "Model/DeckListModel.hpp"
#include "Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Bridge {

Q_INVOKABLE QString DeckBridge::NameLengthErrorMessage() {
    return QString{ "Name length error" };
}

Q_INVOKABLE QString DeckBridge::DuplicateErrorMessage() {
    return QString{ "Name already exists" };
}

[[nodiscard]] QString DeckBridge::HandleQueryResult(
    const std::unique_ptr<duckdb::QueryResult>& QueryResult) {
    switch (QueryResult->GetErrorType()) {
    case duckdb::ExceptionType::INVALID: {
        return QString{};
    }
    case duckdb::ExceptionType::CONSTRAINT: {
        const duckdb::ErrorData& ErrorData{ QueryResult->GetErrorObject() };
        std::string_view RawMessage{ ErrorData.RawMessage() };
        if (RawMessage.starts_with("CHECK")) {
            return NameLengthErrorMessage();
        }
        if (RawMessage.starts_with("Duplicate")) {
            return DuplicateErrorMessage();
        }
        Support::ThrowError(QueryResult->GetError());
    }
    default: {
        Support::ThrowError(QueryResult->GetError());
    }
    }
}

Q_INVOKABLE QString DeckBridge::CreateDeck(const QString& DeckName) {
    return Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{
            m_DatabaseConnection.Query(Sql::CreateDeckSql(),
                                       DeckName.toStdString())
        };
        const QString Error{ HandleQueryResult(QueryResult) };
        if (Error.isEmpty()) {
            ReadDeckTable();
        }
        return Error;
    });
}

Q_INVOKABLE QString
DeckBridge::UpdateDeck(const Model::DeckListModel::DeckItem& DeckItem) {
    return Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{
            m_DatabaseConnection.Query(Sql::UpdateDeckSql(),
                                       DeckItem.m_Id.toStdString(),
                                       DeckItem.m_Name.toStdString())
        };
        const QString Error{ HandleQueryResult(QueryResult) };
        if (Error.isEmpty()) {
            ReadDeckTable();
        }
        return Error;
    });
}

Q_INVOKABLE void DeckBridge::DeleteDeck(const QString& DeckId) {
    Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{
            m_DatabaseConnection.Query(Sql::DeleteDeckSql(),
                                       DeckId.toStdString())
        };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        ReadDeckTable();
    });
}

void DeckBridge::ReadDeckTable() {
    Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{
            m_DatabaseConnection.Query(Sql::ReadDecksTableSql())
        };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        QVector<Model::DeckListModel::DeckItem> DeckList;
        for (auto QueryResultIterator{ QueryResult->begin() };
             QueryResultIterator != QueryResult->end();
             ++QueryResultIterator) {
            const auto& QueryResultRow{ *QueryResultIterator };
            DeckList.emplace_back(
                QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
                QString{ QueryResultRow.GetValue<std::string>(1).c_str() });
        }
        m_DeckList.ReplaceAll(std::move(DeckList));
    });
}
}
