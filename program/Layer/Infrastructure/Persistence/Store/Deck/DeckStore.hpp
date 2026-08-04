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
    Database::PreparedStatement m_CreateRootDeckPreparedStatement;
    Database::PreparedStatement m_CreateChildDeckPreparedStatement;
    Database::PreparedStatement m_MoveDeckToRootPreparedStatement;
    Database::PreparedStatement m_MoveDeckUnderParentPreparedStatement;
    Database::PreparedStatement m_RenameDeckPreparedStatement;
    Database::PreparedStatement m_DeleteDeckCardReviewsPreparedStatement;
    Database::PreparedStatement m_DeleteDeckCardsPreparedStatement;
    Database::PreparedStatement m_DeleteDeckPreparedStatement;

public:
    explicit DeckStore(Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_CreateRootDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::CreateRootDeckSql()) }
        , m_CreateChildDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::CreateChildDeckSql()) }
        , m_MoveDeckToRootPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::MoveDeckToRootSql()) }
        , m_MoveDeckUnderParentPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::MoveDeckUnderParentSql()) }
        , m_RenameDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::RenameDeckSql()) }
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
