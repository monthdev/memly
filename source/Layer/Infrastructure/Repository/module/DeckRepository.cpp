module;

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

module Memly.Repository.DeckRepository;

import Memly.Database.DatabaseRuntime;
import Memly.Database.QueryResultRowCountRange;
import Memly.Repository.DeckSnapshotRecord;
import Memly.Repository.MutatedId;

import Memly.Repository.Internal.DeckSql;

namespace Memly::Repository {

DeckRepository::DeckRepository(Database::DatabaseRuntime& DatabaseRuntime) :
    m_SelectDeckSnapshotRecordsPreparedStatement{
        DatabaseRuntime
            .PrepareStatement(Internal::SelectDeckSnapshotRecordsSql()),
    },
    m_InsertRootDeckPreparedStatement{
        DatabaseRuntime.PrepareStatement(Internal::InsertRootDeckSql()),
    },
    m_InsertChildDeckPreparedStatement{
        DatabaseRuntime.PrepareStatement(Internal::InsertChildDeckSql()),
    },
    m_UpdateDeckParentToRootPreparedStatement{
        DatabaseRuntime.PrepareStatement(Internal::UpdateDeckParentToRootSql()),
    },
    m_UpdateDeckParentPreparedStatement{
        DatabaseRuntime.PrepareStatement(Internal::UpdateDeckParentSql()),
    },
    m_UpdateDeckNamePreparedStatement{
        DatabaseRuntime.PrepareStatement(Internal::UpdateDeckNameSql()),
    },
    m_DeleteDeckCardReviewsPreparedStatement{
        DatabaseRuntime.PrepareStatement(Internal::DeleteDeckCardReviewsSql()),
    },
    m_DeleteDeckCardsPreparedStatement{
        DatabaseRuntime.PrepareStatement(Internal::DeleteDeckCardsSql()),
    },
    m_DeleteDeckPreparedStatement{
        DatabaseRuntime.PrepareStatement(Internal::DeleteDeckSql()),
    } {
}

[[nodiscard]] std::vector<DeckSnapshotRecord>
DeckRepository::ReadDeckSnapshotRecords(
    const std::int64_t AsOfMillisecondsSinceEpoch
) {
    return std::vector<DeckSnapshotRecord>{
        this->m_SelectDeckSnapshotRecordsPreparedStatement.Execute()
            .WithParameters(AsOfMillisecondsSinceEpoch)
            .DecodedTo<DeckSnapshotRecord>()
            .AssertRowCount(Database::QueryResultRowCountRange::ZeroOrMore()),
    };
}

void
DeckRepository::CreateRootDeck(
    const std::string& DeckName,
    const std::uint8_t TargetLanguageCode
) {
    static_cast<void>(this->m_InsertRootDeckPreparedStatement.Execute()
            .WithParameters(DeckName, TargetLanguageCode)
            .DecodedTo<MutatedId>()
            .AssertRowCount(
                Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })
            ));
}

void
DeckRepository::CreateChildDeck(
    const std::string& DeckName,
    const std::string& ParentDeckId
) {
    static_cast<void>(this->m_InsertChildDeckPreparedStatement.Execute()
            .WithParameters(ParentDeckId, DeckName)
            .DecodedTo<MutatedId>()
            .AssertRowCount(
                Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })
            ));
}

void
DeckRepository::MoveDeckToRoot(const std::string& DeckId) {
    static_cast<void>(this->m_UpdateDeckParentToRootPreparedStatement.Execute()
            .WithParameters(DeckId)
            .DecodedTo<MutatedId>()
            .AssertRowCount(
                Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })
            ));
}

void
DeckRepository::MoveDeckUnderParent(
    const std::string& DeckId,
    const std::string& ParentDeckId
) {
    static_cast<void>(this->m_UpdateDeckParentPreparedStatement.Execute()
            .WithParameters(DeckId, ParentDeckId)
            .DecodedTo<MutatedId>()
            .AssertRowCount(
                Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })
            ));
}

void
DeckRepository::RenameDeck(
    const std::string& DeckId,
    const std::string& NewDeckName
) {
    static_cast<void>(this->m_UpdateDeckNamePreparedStatement.Execute()
            .WithParameters(NewDeckName, DeckId)
            .DecodedTo<MutatedId>()
            .AssertRowCount(
                Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })
            ));
}

void
DeckRepository::DeleteDeck(const std::string& DeckId) {
    static_cast<void>(this->m_DeleteDeckCardReviewsPreparedStatement.Execute()
            .WithParameters(DeckId)
            .DecodedTo<MutatedId>()
            .AssertRowCount(Database::QueryResultRowCountRange::ZeroOrMore()));
    static_cast<void>(this->m_DeleteDeckCardsPreparedStatement.Execute()
            .WithParameters(DeckId)
            .DecodedTo<MutatedId>()
            .AssertRowCount(Database::QueryResultRowCountRange::ZeroOrMore()));
    static_cast<void>(this->m_DeleteDeckPreparedStatement.Execute()
            .WithParameters(DeckId)
            .DecodedTo<MutatedId>()
            .AssertRowCount(
                Database::QueryResultRowCountRange::AtLeast(std::size_t{ 1 })
            ));
}

}
