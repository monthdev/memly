// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "Layer/Application/Domain/ReviewSession/RecoverableReviewSessionMutationError.hpp"
#include "Layer/Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"
#include "Layer/Infrastructure/Persistence/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Persistence/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Persistence/Sql/ReviewSession/ReviewSessionSql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Persistence::Store::ReviewSession {

class ReviewSessionStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
    Database::PreparedStatement m_InsertCustomReviewSessionPreparedStatement;
    Database::PreparedStatement m_InsertDefaultReviewSessionPreparedStatement;
    Database::PreparedStatement m_InsertCustomReviewSessionDeckSelectionPreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionNamePreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionToDefaultPreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionToCustomPreparedStatement;
    Database::PreparedStatement m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement;
    Database::PreparedStatement m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement;
    Database::PreparedStatement m_DeleteReviewSessionPreparedStatement;
    Database::PreparedStatement m_SelectDefaultReviewSessionIdByRootDeckIdPreparedStatement;
    Database::PreparedStatement m_SelectReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement;

public:
    explicit ReviewSessionStore(Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseRuntime{ DatabaseRuntime }
        , m_InsertCustomReviewSessionPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::InsertCustomReviewSessionSql())
        }
        , m_InsertDefaultReviewSessionPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::InsertDefaultReviewSessionSql())
        }
        , m_InsertCustomReviewSessionDeckSelectionPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::InsertCustomReviewSessionDeckSelectionSql())
        }
        , m_UpdateReviewSessionNamePreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::ReviewSession::UpdateReviewSessionNameSql()) }
        , m_UpdateReviewSessionToDefaultPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::UpdateReviewSessionToDefaultSql())
        }
        , m_UpdateReviewSessionToCustomPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::UpdateReviewSessionToCustomSql())
        }
        , m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement{ DatabaseRuntime.PrepareStatement(
              Sql::ReviewSession::UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql()) }
        , m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::DeleteCustomReviewSessionDeckSelectionsSql())
        }
        , m_DeleteReviewSessionPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::ReviewSession::DeleteReviewSessionSql()) }
        , m_SelectDefaultReviewSessionIdByRootDeckIdPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::SelectDefaultReviewSessionIdByRootDeckIdSql())
        }
        , m_SelectReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::ReviewSession::SelectReviewSessionIdByReviewSessionDefinitionKeySql())
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
