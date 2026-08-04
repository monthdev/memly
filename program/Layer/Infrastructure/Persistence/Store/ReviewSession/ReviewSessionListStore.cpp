// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Infrastructure/Persistence/Store/ReviewSession/ReviewSessionListStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Layer/Application/Domain/ReviewSession/ReviewSessionListRow.hpp"
#include "Layer/Infrastructure/Persistence/Database/DatabaseRuntime.hpp"

namespace Layer::Infrastructure::Persistence::Store::ReviewSession {

[[nodiscard]] auto ReviewSessionListStore::ReadReviewSessionListRows() -> std::vector<Application::Domain::ReviewSession::ReviewSessionListRow> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{
        this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_SelectReviewSessionListRowsPreparedStatement).WithoutParameters()
    };
    std::vector<Application::Domain::ReviewSession::ReviewSessionListRow> ReviewSessionListRowVector{};
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ this->m_DatabaseRuntime.FetchNextDataChunk(*QueryResult) }) {
        for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size(); ++RowIndex) {
            const duckdb::Value LastUpdatedAtMillisecondsSinceEpochValue{ DataChunk->GetValue(3, RowIndex) };
            const duckdb::Value LastCardReviewAtMillisecondsSinceEpochValue{ DataChunk->GetValue(4, RowIndex) };
            ReviewSessionListRowVector.emplace_back(
                DataChunk->GetValue(0, RowIndex).GetValue<std::string>(),
                DataChunk->GetValue(1, RowIndex).GetValue<std::string>(),
                DataChunk->GetValue(2, RowIndex).GetValue<std::int64_t>(),
                LastUpdatedAtMillisecondsSinceEpochValue.IsNull() ? std::nullopt :
                                                                    std::make_optional(LastUpdatedAtMillisecondsSinceEpochValue.GetValue<std::int64_t>()),
                LastCardReviewAtMillisecondsSinceEpochValue.IsNull() ?
                    std::nullopt :
                    std::make_optional(LastCardReviewAtMillisecondsSinceEpochValue.GetValue<std::int64_t>()));
        }
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return ReviewSessionListRowVector;
}

}
#endif
