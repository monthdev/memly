#pragma once

#include <QString>
#include <QtGlobal>
#include <memory>
#include <optional>

namespace duckdb {
class Connection;
class PreparedStatement;
class QueryResult;
}

namespace Infrastructure::Store {

class DeckStore final {
public:
    enum class RecoverableDeckMutationErrorEnum : std::uint8_t {
        DeckNameLengthError,
        DuplicateDeckNameError,
        InvalidTargetLanguageCodeError,
        ParentDeckTargetLanguageMismatchError,
        DeckTreeCycleDetectionError
    };

    explicit DeckStore(duckdb::Connection&);
    ~DeckStore();

    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> CreateRootDeck(const QString&, quint8);
    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> CreateChildDeck(const QString&, const QString&);
    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> MoveDeck(const QString&, const std::optional<QString>&);
    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> RenameDeck(const QString&, const QString&);
    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> DeleteDeck(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateRootDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateChildDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_MoveDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_RenameDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardReviewsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckPreparedStatement;

    [[nodiscard]] std::optional<RecoverableDeckMutationErrorEnum> HandleRecoverableDeckMutationError(duckdb::QueryResult&) const;
};

}
