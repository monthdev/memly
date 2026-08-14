#include "Memly/Repository/DeckRepository.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "Memly/Database/DatabaseRuntime.hpp"
#include "Memly/Database/QueryResultRowCountRange.hpp"
#include "Memly/Repository/DeckSnapshotRecord.hpp"
#include "Memly/Repository/MutatedId.hpp"
#include "_Sql/_DeckSql.hpp"

namespace Memly::Repository {

DeckRepository::DeckRepository(Database::DatabaseRuntime& DatabaseRuntime)
    : m_SelectDeckSnapshotRecordsPreparedStatement{ DatabaseRuntime.PrepareStatement(i_SelectDeckSnapshotRecordsSql()) }
    , m_InsertRootDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(i_InsertRootDeckSql()) }
    , m_InsertChildDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(i_InsertChildDeckSql()) }
    , m_UpdateDeckParentToRootPreparedStatement{ DatabaseRuntime.PrepareStatement(i_UpdateDeckParentToRootSql()) }
    , m_UpdateDeckParentPreparedStatement{ DatabaseRuntime.PrepareStatement(i_UpdateDeckParentSql()) }
    , m_UpdateDeckNamePreparedStatement{ DatabaseRuntime.PrepareStatement(i_UpdateDeckNameSql()) }
    , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseRuntime.PrepareStatement(i_DeleteDeckCardReviewsSql()) }
    , m_DeleteDeckCardsPreparedStatement{ DatabaseRuntime.PrepareStatement(i_DeleteDeckCardsSql()) }
    , m_DeleteDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(i_DeleteDeckSql()) } {
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
