#pragma once

#include <cstdint>
#include <string>

#include "Layer/Infrastructure/Persistence/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Persistence/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Persistence/Sql/Deck/DeckSql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Persistence::Store::Deck {

class DeckStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::PreparedStatement m_InsertRootDeckPreparedStatement;
    Database::PreparedStatement m_InsertChildDeckPreparedStatement;
    Database::PreparedStatement m_UpdateDeckParentToRootPreparedStatement;
    Database::PreparedStatement m_UpdateDeckParentPreparedStatement;
    Database::PreparedStatement m_UpdateDeckNamePreparedStatement;
    Database::PreparedStatement m_DeleteDeckCardReviewsPreparedStatement;
    Database::PreparedStatement m_DeleteDeckCardsPreparedStatement;
    Database::PreparedStatement m_DeleteDeckPreparedStatement;

public:
    explicit DeckStore(Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_InsertRootDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::InsertRootDeckSql()) }
        , m_InsertChildDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::InsertChildDeckSql()) }
        , m_UpdateDeckParentToRootPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::UpdateDeckParentToRootSql()) }
        , m_UpdateDeckParentPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::UpdateDeckParentSql()) }
        , m_UpdateDeckNamePreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::UpdateDeckNameSql()) }
        , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::DeleteDeckCardReviewsSql()) }
        , m_DeleteDeckCardsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::DeleteDeckCardsSql()) }
        , m_DeleteDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::DeleteDeckSql()) } {
    }

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeckToRoot(const std::string&);
    void MoveDeckUnderParent(const std::string&, const std::string&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
};

}
