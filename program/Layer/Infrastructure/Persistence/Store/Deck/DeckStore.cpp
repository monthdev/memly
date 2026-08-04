#include "Layer/Infrastructure/Persistence/Store/Deck/DeckStore.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

#include "Layer/Infrastructure/Persistence/Database/QueryResultRowCountRange.hpp"
#include "Layer/Infrastructure/Persistence/Store/MutatedId.hpp"

namespace Layer::Infrastructure::Persistence::Store::Deck {

void DeckStore::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) {
    static_cast<void>(this->m_InsertRootDeckPreparedStatement.Execute()
                          .WithParameters(DeckName, TargetLanguageCode)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) {
    static_cast<void>(this->m_InsertChildDeckPreparedStatement.Execute()
                          .WithParameters(ParentDeckId, DeckName)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::MoveDeckToRoot(const std::string& DeckId) {
    static_cast<void>(this->m_UpdateDeckParentToRootPreparedStatement.Execute().WithParameters(DeckId).DecodedTo<MutatedId>().AssertRowCount(
        Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::MoveDeckUnderParent(const std::string& DeckId, const std::string& ParentDeckId) {
    static_cast<void>(this->m_UpdateDeckParentPreparedStatement.Execute()
                          .WithParameters(DeckId, ParentDeckId)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) {
    static_cast<void>(this->m_UpdateDeckNamePreparedStatement.Execute()
                          .WithParameters(NewDeckName, DeckId)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::DeleteDeck(const std::string& DeckId) {
    static_cast<void>(this->m_DeleteDeckCardReviewsPreparedStatement.Execute().WithParameters(DeckId));
    static_cast<void>(this->m_DeleteDeckCardsPreparedStatement.Execute().WithParameters(DeckId));
    static_cast<void>(this->m_DeleteDeckPreparedStatement.Execute().WithParameters(DeckId).DecodedTo<MutatedId>().AssertRowCount(
        Database::QueryResultRowCountRange::AtLeast(std::size_t{ 1 })));
}

}
