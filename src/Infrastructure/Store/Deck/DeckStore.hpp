#pragma once

#include <cstdint>
#include <string>

#include "Infrastructure/Database/DatabaseRuntime.hpp"
#include "Infrastructure/Database/PreparedStatement.hpp"
#include "Infrastructure/Sql/Deck/Mutation/DeckMutationSql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::Deck {

class DeckStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::Database::DatabaseRuntime& m_DatabaseRuntime;
    Infrastructure::Database::PreparedStatement m_CreateRootDeckPreparedStatement;
    Infrastructure::Database::PreparedStatement m_CreateChildDeckPreparedStatement;
    Infrastructure::Database::PreparedStatement m_MoveDeckToRootPreparedStatement;
    Infrastructure::Database::PreparedStatement m_MoveDeckUnderParentPreparedStatement;
    Infrastructure::Database::PreparedStatement m_RenameDeckPreparedStatement;
    Infrastructure::Database::PreparedStatement m_DeleteDeckCardReviewsPreparedStatement;
    Infrastructure::Database::PreparedStatement m_DeleteDeckCardsPreparedStatement;
    Infrastructure::Database::PreparedStatement m_DeleteDeckPreparedStatement;

public:
    explicit DeckStore(Infrastructure::Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseRuntime{ DatabaseRuntime }
        , m_CreateRootDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Mutation::CreateRootDeckSql()) }
        , m_CreateChildDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Mutation::CreateChildDeckSql()) }
        , m_MoveDeckToRootPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Mutation::MoveDeckToRootSql()) }
        , m_MoveDeckUnderParentPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Mutation::MoveDeckUnderParentSql()) }
        , m_RenameDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Mutation::RenameDeckSql()) }
        , m_DeleteDeckCardReviewsPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Mutation::DeleteDeckCardReviewsSql()) }
        , m_DeleteDeckCardsPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Mutation::DeleteDeckCardsSql()) }
        , m_DeleteDeckPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Mutation::DeleteDeckSql()) } {
    }

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeckToRoot(const std::string&);
    void MoveDeckUnderParent(const std::string&, const std::string&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
};

}
