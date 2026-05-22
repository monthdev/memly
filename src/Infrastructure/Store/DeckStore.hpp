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
    enum class DeckMutationErrorEnum : std::uint8_t {
        DeckNameLengthError,
        DuplicateDeckNameError,
        InvalidTargetLanguageCodeError,
        ParentDeckTargetLanguageMismatchError,
        DeckTreeCycleDetectionError
    };

    explicit DeckStore(duckdb::Connection&);
    ~DeckStore();

    [[nodiscard]] std::optional<DeckMutationErrorEnum> CreateRootDeck(const QString&, quint8);
    [[nodiscard]] std::optional<DeckMutationErrorEnum> CreateChildDeck(const QString&, const QString&);
    [[nodiscard]] std::optional<DeckMutationErrorEnum> MoveDeck(const QString&, const std::optional<QString>&);
    [[nodiscard]] std::optional<DeckMutationErrorEnum> RenameDeck(const QString&, const QString&);
    [[nodiscard]] std::optional<DeckMutationErrorEnum> DeleteDeck(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateRootDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_CreateChildDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_MoveDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_RenameDeckPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardReviewsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckCardsPreparedStatement;
    std::unique_ptr<duckdb::PreparedStatement> m_DeleteDeckPreparedStatement;

    [[nodiscard]] std::optional<DeckMutationErrorEnum> HandleRequiredDeckMutation(const std::unique_ptr<duckdb::QueryResult>&, const std::string_view) const;
    [[nodiscard]] std::optional<DeckMutationErrorEnum> HandleDeckMutationError(const std::unique_ptr<duckdb::QueryResult>&) const;
};

}
