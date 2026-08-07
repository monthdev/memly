#include "Layer/Infrastructure/DuckDb/Repository/Deck/DeckRepository.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "Layer/Infrastructure/DuckDb/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/DuckDb/Database/QueryResultRowCountRange.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/Deck/DeckSnapshotRecord.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/Deck/Sql/DeckSql.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/MutatedId.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Repository::Deck {

DeckRepository::DeckRepository(Database::DatabaseRuntime& DatabaseRuntime)
    : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_SelectDeckSnapshotRecordsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::SelectDeckSnapshotRecordsSql()) }
    , m_InsertRootDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::InsertRootDeckSql()) }
    , m_InsertChildDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::InsertChildDeckSql()) }
    , m_UpdateDeckParentToRootPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::UpdateDeckParentToRootSql()) }
    , m_UpdateDeckParentPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::UpdateDeckParentSql()) }
    , m_UpdateDeckNamePreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::UpdateDeckNameSql()) }
    , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::DeleteDeckCardReviewsSql()) }
    , m_DeleteDeckCardsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::DeleteDeckCardsSql()) }
    , m_DeleteDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::DeleteDeckSql()) } {
}

[[nodiscard]] auto DeckRepository::ReadDeckSnapshotRecords(const std::int64_t AsOfMillisecondsSinceEpoch) -> std::vector<DeckSnapshotRecord> {
    return std::vector<DeckSnapshotRecord>{ this->m_SelectDeckSnapshotRecordsPreparedStatement.Execute()
                                                .WithParameters(AsOfMillisecondsSinceEpoch)
                                                .DecodedTo<DeckSnapshotRecord>()
                                                .AssertRowCount(Database::QueryResultRowCountRange::ZeroOrMore()) };
}

void DeckRepository::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) {
    static_cast<void>(this->m_InsertRootDeckPreparedStatement.Execute()
                          .WithParameters(DeckName, TargetLanguageCode)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckRepository::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) {
    static_cast<void>(this->m_InsertChildDeckPreparedStatement.Execute()
                          .WithParameters(ParentDeckId, DeckName)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckRepository::MoveDeckToRoot(const std::string& DeckId) {
    static_cast<void>(this->m_UpdateDeckParentToRootPreparedStatement.Execute().WithParameters(DeckId).DecodedTo<MutatedId>().AssertRowCount(
        Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckRepository::MoveDeckUnderParent(const std::string& DeckId, const std::string& ParentDeckId) {
    static_cast<void>(this->m_UpdateDeckParentPreparedStatement.Execute()
                          .WithParameters(DeckId, ParentDeckId)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckRepository::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) {
    static_cast<void>(this->m_UpdateDeckNamePreparedStatement.Execute()
                          .WithParameters(NewDeckName, DeckId)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckRepository::DeleteDeck(const std::string& DeckId) {
    static_cast<void>(this->m_DeleteDeckCardReviewsPreparedStatement.Execute().WithParameters(DeckId));
    static_cast<void>(this->m_DeleteDeckCardsPreparedStatement.Execute().WithParameters(DeckId));
    static_cast<void>(this->m_DeleteDeckPreparedStatement.Execute().WithParameters(DeckId).DecodedTo<MutatedId>().AssertRowCount(
        Database::QueryResultRowCountRange::AtLeast(std::size_t{ 1 })));
}

}
