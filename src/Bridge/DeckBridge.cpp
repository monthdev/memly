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
        ReadDeckTable();
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
        return HandleQueryResult(QueryResult);
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
        return HandleQueryResult(QueryResult);
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
    });
}

void DeckBridge::ReadDeckTable() {
    Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{
            m_DatabaseConnection.Query(Sql::ReadDeckTableSql())
        };
        switch (QueryResult->GetErrorType()) {
        case duckdb::ExceptionType::INVALID: {
            QVector<Model::DeckListModel::DeckItem> Decks;
            while (true) {
                std::unique_ptr<duckdb::DataChunk> DataChunk =
                    QueryResult->Fetch();
                if (!DataChunk || DataChunk->size() == 0) {
                    break;
                }
                for (duckdb::idx_t Row{ 0 }; Row < DataChunk->size(); ++Row) {
                    Decks.push_back(Model::DeckListModel::DeckItem{
                        .m_Id = QString::fromStdString(
                            DataChunk->GetValue(0, Row).ToString()),
                        .m_Name = QString::fromStdString(
                            DataChunk->GetValue(1, Row).ToString()),
                    });
                }
            }
            m_DeckList.ReplaceAll(std::move(Decks));
            break;
        }
        default: {
            Support::ThrowError(QueryResult->GetError());
        }
        }
    });
}
}
