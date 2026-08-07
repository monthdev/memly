#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Layer/Infrastructure/DuckDb/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/Deck/DeckSnapshotRecord.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {
class DatabaseRuntime;
}

namespace Layer::Infrastructure::DuckDb::Repository::Deck {

class DeckRepository final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::PreparedStatement m_SelectDeckSnapshotRecordsPreparedStatement;
    Database::PreparedStatement m_InsertRootDeckPreparedStatement;
    Database::PreparedStatement m_InsertChildDeckPreparedStatement;
    Database::PreparedStatement m_UpdateDeckParentToRootPreparedStatement;
    Database::PreparedStatement m_UpdateDeckParentPreparedStatement;
    Database::PreparedStatement m_UpdateDeckNamePreparedStatement;
    Database::PreparedStatement m_DeleteDeckCardReviewsPreparedStatement;
    Database::PreparedStatement m_DeleteDeckCardsPreparedStatement;
    Database::PreparedStatement m_DeleteDeckPreparedStatement;

public:
    explicit DeckRepository(Database::DatabaseRuntime&);

    [[nodiscard]] auto ReadDeckSnapshotRecords(std::int64_t) -> std::vector<DeckSnapshotRecord>;
    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeckToRoot(const std::string&);
    void MoveDeckUnderParent(const std::string&, const std::string&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
};

}
