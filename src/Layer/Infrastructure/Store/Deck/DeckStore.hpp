#pragma once

#include <cstdint>
#include <string>

#include "Layer/Infrastructure/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Sql/Deck/Mutation/DeckMutationSql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Store::Deck {

class DeckStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
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
        , m_DatabaseRuntime{ DatabaseRuntime }
        , m_CreateRootDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Mutation::CreateRootDeckSql()) }
        , m_CreateChildDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Mutation::CreateChildDeckSql()) }
        , m_MoveDeckToRootPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Mutation::MoveDeckToRootSql()) }
        , m_MoveDeckUnderParentPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Mutation::MoveDeckUnderParentSql()) }
        , m_RenameDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Mutation::RenameDeckSql()) }
        , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Mutation::DeleteDeckCardReviewsSql()) }
        , m_DeleteDeckCardsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Mutation::DeleteDeckCardsSql()) }
        , m_DeleteDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Mutation::DeleteDeckSql()) } {
    }

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeckToRoot(const std::string&);
    void MoveDeckUnderParent(const std::string&, const std::string&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
};

}
