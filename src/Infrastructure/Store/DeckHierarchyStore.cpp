#include "Infrastructure/Store/DeckHierarchyStore.hpp"

#include <duckdb.hpp>

#include <memory>

#include "Infrastructure/Sql/SqlResource.hpp"
#include "Infrastructure/Store/PreTreeDeckHierarchyValidation.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

DeckHierarchyStore::DeckHierarchyViewSnapshot DeckHierarchyStore::ReadDeckHierarchyViewSnapshot() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Infrastructure::Sql::ReadDeckHierarchyViewSnapshotSql()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    DeckHierarchyViewSnapshot DeckHierarchyViewSnapshot{};
    bool NextRefreshAtMillisecondsSinceEpochValueSeen{ false };
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        DeckHierarchyViewSnapshot.m_DeckHierarchyRowQVector.emplace_back(QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
                                                                         QString{ QueryResultRow.GetValue<std::string>(1).c_str() },
                                                                         QString{ QueryResultRow.GetValue<std::string>(2).c_str() },
                                                                         static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(3)),
                                                                         static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(4)),
                                                                         static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(5)),
                                                                         static_cast<quint8>(QueryResultRow.GetValue<std::uint8_t>(6)));
        if (not NextRefreshAtMillisecondsSinceEpochValueSeen) {
            std::int64_t NextRefreshAtMillisecondsSinceEpoch = QueryResultRow.GetValue<std::int64_t>(7);
            if (NextRefreshAtMillisecondsSinceEpoch > 0) {
                DeckHierarchyViewSnapshot.m_NextRefreshAtMillisecondsSinceEpoch = NextRefreshAtMillisecondsSinceEpoch;
            }
            NextRefreshAtMillisecondsSinceEpochValueSeen = true;
        }
    }
    ValidatePreTreeDeckHierarchy(DeckHierarchyViewSnapshot.m_DeckHierarchyRowQVector);
    return DeckHierarchyViewSnapshot;
}
}
