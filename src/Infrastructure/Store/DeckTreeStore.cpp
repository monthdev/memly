#include "Infrastructure/Store/DeckTreeStore.hpp"

#include <duckdb.hpp>

#include <memory>

#include "Infrastructure/Sql/SqlResource.hpp"
#include "Infrastructure/Store/PreDeckTreeValidation.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

[[nodiscard]] QVector<DeckTreeStore::DeckTreeRow> DeckTreeStore::ReadDeckTreeSnapshot(const qint64 AsOfMillisecondsSinceEpoch) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Infrastructure::Sql::ReadDeckTreeSnapshotSql(), AsOfMillisecondsSinceEpoch) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    QVector<DeckTreeRow> DeckTreeRowQVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        DeckTreeRowQVector.emplace_back(QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
                                        QString{ QueryResultRow.GetValue<std::string>(1).c_str() },
                                        QString{ QueryResultRow.GetValue<std::string>(2).c_str() },
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(3)),
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(4)),
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(5)),
                                        static_cast<quint8>(QueryResultRow.GetValue<std::uint8_t>(6)));
    }
    ValidatePreDeckTree(DeckTreeRowQVector);
    return DeckTreeRowQVector;
}
}
