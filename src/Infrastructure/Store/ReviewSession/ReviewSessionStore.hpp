#pragma once

#include <QString>
#include <QVector>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>

namespace duckdb {
class Connection;
class PreparedStatement;
class QueryResult;
}

namespace Infrastructure::Store::ReviewSession {

class ReviewSessionStore final {
public:
    enum class RecoverableReviewSessionMutationErrorEnum : std::uint8_t {
        ReviewSessionNameLengthError,
        DuplicateReviewSessionDefinitionKeyError,
        ConflictingReviewSessionDeckSelfSelectionError,
        ConflictingReviewSessionDeckSubtreeSelectionError,
        ConflictingReviewSessionDeckIncludeSelectionError,
        ConflictingReviewSessionDeckExcludeSelectionError
    };

    enum class ReviewSessionDeckSelectionTypeEnum : std::uint8_t {
        Self,
        Subtree,
        ExcludeSelf,
        ExcludeSubtree
    };

    struct ReviewSessionDeckSelection {
        QString m_DeckId;
        ReviewSessionDeckSelectionTypeEnum m_DeckSelectionType;
    };

    explicit ReviewSessionStore(duckdb::Connection&);
    ~ReviewSessionStore();

    [[nodiscard]] std::expected<QString, RecoverableReviewSessionMutationErrorEnum> CreateOrReadExistingDefaultReviewSession(const QString&, const QString&);
    [[nodiscard]] std::expected<QString, RecoverableReviewSessionMutationErrorEnum>
    CreateOrReadExistingCustomReviewSession(const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    [[nodiscard]] std::optional<RecoverableReviewSessionMutationErrorEnum> RenameReviewSession(const QString&, const QString&);
    [[nodiscard]] std::expected<QString, RecoverableReviewSessionMutationErrorEnum> EditReviewSessionToDefault(const QString&, const QString&, const QString&);
    [[nodiscard]] std::expected<QString, RecoverableReviewSessionMutationErrorEnum>
    EditReviewSessionToCustom(const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    void UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const QString&);
    void DeleteReviewSession(const QString&);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_CreateCustomReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateDefaultReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateCustomReviewSessionDeckSelectionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_RenameReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_UpdateReviewSessionToDefaultPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_UpdateReviewSessionToCustomPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteCustomReviewSessionDeckSelectionsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_ReadDefaultReviewSessionIdByRootDeckIdPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement;

    [[nodiscard]] std::optional<QString> TryReadDefaultReviewSessionIdByRootDeckId(const QString&);
    [[nodiscard]] std::optional<QString> TryReadReviewSessionIdByReviewSessionDefinitionKey(const QString&);
    [[nodiscard]] std::optional<RecoverableReviewSessionMutationErrorEnum>
    CreateCustomReviewSessionDeckSelection(const QString&, const QString&, ReviewSessionDeckSelectionTypeEnum);
    void DeleteCustomReviewSessionDeckSelections(const QString&);
    [[nodiscard]] std::optional<RecoverableReviewSessionMutationErrorEnum> HandleRecoverableReviewSessionMutationError(duckdb::QueryResult&) const;
};

}
