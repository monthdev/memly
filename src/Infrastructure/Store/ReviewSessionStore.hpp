#pragma once

#include <QString>
#include <QVector>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <string_view>

namespace duckdb {
class Connection;
class PreparedStatement;
class QueryResult;
}

namespace Infrastructure::Store {

class ReviewSessionStore final {
public:
    enum class ReviewSessionMutationErrorEnum : std::uint8_t {
        ReviewSessionNameLengthError,
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

    [[nodiscard]] std::expected<QString, ReviewSessionMutationErrorEnum>
    CreateOrReadExistingReviewSession(const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    [[nodiscard]] std::expected<QString, ReviewSessionMutationErrorEnum>
    EditReviewSessionDeckSelections(const QString&, const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    void UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch(const QString&);
    void DeleteReviewSession(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateReviewSessionDeckSelectionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteReviewSessionDeckSelectionsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteReviewSessionPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_ReadReviewSessionIdByReviewSessionDefinitionKeyPreparedStatement;

    [[nodiscard]] std::optional<QString> TryReadReviewSessionIdByReviewSessionDefinitionKey(const QString&);
    [[nodiscard]] std::expected<QString, ReviewSessionMutationErrorEnum>
    CreateOrReadExistingReviewSessionInCurrentTransaction(const QString&, const QString&, const QVector<ReviewSessionDeckSelection>&);
    [[nodiscard]] std::expected<QString, ReviewSessionMutationErrorEnum> CreateReviewSession(const QString&, const QString&);
    [[nodiscard]] std::optional<ReviewSessionMutationErrorEnum>
    CreateReviewSessionDeckSelection(const QString&, const QString&, ReviewSessionDeckSelectionTypeEnum);
    void HandleRequiredReviewSessionMutation(const std::unique_ptr<duckdb::QueryResult>&, const std::string_view) const;
    void DeleteReviewSessionInCurrentTransaction(const QString&);
    [[nodiscard]] std::optional<ReviewSessionMutationErrorEnum> HandleReviewSessionMutationError(const std::unique_ptr<duckdb::QueryResult>&) const;
};

}
