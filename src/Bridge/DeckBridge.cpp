#include "Bridge/DeckBridge.hpp"

#include <duckdb.hpp>

#include <memory>

#include "Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Bridge {

Q_INVOKABLE QString DeckBridge::NameLengthErrorMessage() {
    return QString{ "Name length error" };
}

Q_INVOKABLE QString DeckBridge::DuplicateErrorMessage() {
    return QString{ "Name already exists" };
}

Q_INVOKABLE QString DeckBridge::AddDeck(const QString& Name) {
    return Support::TryCatchWrapper([&] {
        std::unique_ptr<duckdb::QueryResult> QueryResult{
            m_DatabaseConnection.Query(Sql::AddDeckStatement(),
                                       Name.toStdString())
        };
        switch (QueryResult->GetErrorType()) {
        case duckdb::ExceptionType::INVALID_TYPE: {
            return QString{};
        }
        case duckdb::ExceptionType::CONSTRAINT: {
            duckdb::ErrorData& ErrorData{ QueryResult->GetErrorObject() };
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
    });
}

// Q_INVOKABLE QString DeckBridge::EditDeck(const QVariantMap& Deck) {
// }

}
