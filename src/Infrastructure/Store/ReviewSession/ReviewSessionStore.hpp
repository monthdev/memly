// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Infrastructure/Database/DatabaseRuntime.hpp"
#include "Infrastructure/Database/PreparedStatement.hpp"
#include "Infrastructure/Sql/ReviewSession/Mutation/ReviewSessionMutationSql.hpp"
#include "Infrastructure/Sql/ReviewSession/Query/ReviewSessionQuerySql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::Database::DatabaseRuntime& m_DatabaseRuntime;
    Infrastructure::Database::PreparedStatement m_CreateCustomReviewSessionPreparedStatement;
    Infrastructure::Database::PreparedStatement m_CreateDefaultReviewSessionPreparedStatement;
    Infrastructure::Database::PreparedStatement m_CreateCustomReviewSessionDeckSelectionPreparedStatement;
    Infrastructure::Database::PreparedStatement m_RenameReviewSessionPreparedStatement;
    Infrastructure::Database::PreparedStatement m_UpdateReviewSessionToDefaultPreparedStatement;
    Infrastructure::Database::PreparedStatement m_UpdateReviewSessionToCustomPreparedStatement;
    Infrastructure::Database::PreparedStatement m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement;
    Infrastructure::Database::PreparedStatement m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement;
    Infrastructure::Database::PreparedStatement m_DeleteReviewSessionPreparedStatement;
    Infrastructure::Database::PreparedStatement m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement;
    Infrastructure::Database::PreparedStatement m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement;

public:
    explicit ReviewSessionStore(Infrastructure::Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseRuntime{ DatabaseRuntime }
        , m_CreateCustomReviewSessionPreparedStatement{
            DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Mutation::CreateCustomReviewSessionSql())
        }
        , m_CreateDefaultReviewSessionPreparedStatement{
            DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Mutation::CreateDefaultReviewSessionSql())
        }
        , m_CreateCustomReviewSessionDeckSelectionPreparedStatement{
            DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Mutation::CreateCustomReviewSessionDeckSelectionSql())
        }
        , m_RenameReviewSessionPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Mutation::RenameReviewSessionSql()) }
        , m_UpdateReviewSessionToDefaultPreparedStatement{
            DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Mutation::UpdateReviewSessionToDefaultSql())
        }
        , m_UpdateReviewSessionToCustomPreparedStatement{
            DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Mutation::UpdateReviewSessionToCustomSql())
        }
        , m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement{ DatabaseRuntime.PrepareStatement(
              Infrastructure::Sql::ReviewSession::Mutation::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql()) }
        , m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement{
            DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Mutation::DeleteCustomReviewSessionDeckSelectionsSql())
        }
        , m_DeleteReviewSessionPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Mutation::DeleteReviewSessionSql()) }
        , m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement{
            DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Query::ReadDefaultReviewSessionIdByRootDeckIdSql())
        }
        , m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement{
            DatabaseRuntime.PrepareStatement(Infrastructure::Sql::ReviewSession::Query::ReadReviewSessionIdByReviewSessionDefinitionKeySql())
        } {
    }

    [[nodiscard]] auto CreateOrReadExistingDefaultReviewSession(const std::string&, const std::string&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto CreateOrReadExistingCustomReviewSession(const std::string&,
                                                               const std::string&,
                                                               const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto RenameReviewSession(const std::string&, const std::string&)
        -> std::optional<Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto EditReviewSessionToDefault(const std::string&, const std::string&, const std::string&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    [[nodiscard]] auto
    EditReviewSessionToCustom(const std::string&, const std::string&, const std::vector<Application::Domain::ReviewSession::ReviewSessionDeckSelection>&)
        -> std::expected<std::string, Application::Domain::ReviewSession::RecoverableReviewSessionMutationErrorEnum>;
    void UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const std::string&);
    void DeleteReviewSession(const std::string&);

private:
    [[nodiscard]] auto TryReadDefaultReviewSessionIdByRootDeckId(const std::string&) -> std::optional<std::string>;
    [[nodiscard]] auto TryReadReviewSessionIdByReviewSessionDefinitionKey(const std::string&) -> std::optional<std::string>;
    void CreateCustomReviewSessionDeckSelection(const std::string&,
                                                const std::string&,
                                                Application::Domain::ReviewSession::ReviewSessionDeckSelection::DeckSelectionTypeEnum);
    void DeleteCustomReviewSessionDeckSelections(const std::string&);
};

}
#endif
