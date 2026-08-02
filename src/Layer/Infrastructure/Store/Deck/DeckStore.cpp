#include "Layer/Infrastructure/Store/Deck/DeckStore.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

#include "Layer/Infrastructure/Database/QueryResultRowCountRange.hpp"
#include "Layer/Infrastructure/Store/MutatedId.hpp"

namespace Layer::Infrastructure::Store::Deck {

void DeckStore::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) {
    static_cast<void>(m_CreateRootDeckPreparedStatement.Execute()
                          .WithParameters(DeckName, TargetLanguageCode)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) {
    static_cast<void>(m_CreateChildDeckPreparedStatement.Execute()
                          .WithParameters(ParentDeckId, DeckName)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::MoveDeckToRoot(const std::string& DeckId) {
    static_cast<void>(m_MoveDeckToRootPreparedStatement.Execute().WithParameters(DeckId).DecodedTo<MutatedId>().AssertRowCount(
        Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::MoveDeckUnderParent(const std::string& DeckId, const std::string& ParentDeckId) {
    static_cast<void>(m_MoveDeckUnderParentPreparedStatement.Execute()
                          .WithParameters(DeckId, ParentDeckId)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) {
    static_cast<void>(m_RenameDeckPreparedStatement.Execute()
                          .WithParameters(NewDeckName, DeckId)
                          .DecodedTo<MutatedId>()
                          .AssertRowCount(Database::QueryResultRowCountRange::Exactly(std::size_t{ 1 })));
}

void DeckStore::DeleteDeck(const std::string& DeckId) {
    static_cast<void>(m_DeleteDeckCardReviewsPreparedStatement.Execute().WithParameters(DeckId));
    static_cast<void>(m_DeleteDeckCardsPreparedStatement.Execute().WithParameters(DeckId));
    static_cast<void>(m_DeleteDeckPreparedStatement.Execute().WithParameters(DeckId).DecodedTo<MutatedId>().AssertRowCount(
        Database::QueryResultRowCountRange::AtLeast(std::size_t{ 1 })));
}

}
