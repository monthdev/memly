#pragma once

#include <QString>
#include <QtGlobal>
#include <memory>
#include <optional>

namespace duckdb {
class Connection;
class QueryResult;
}

namespace Infrastructure::Store {

class DeckStore final {
public:
    enum class DeckMutationErrorEnum : std::uint8_t {
        NameLengthError,
        DuplicateNameError,
        TargetLanguageCodeError,
        ParentDeckError,
        ParentDeckTargetLanguageMismatchError,
        CycleDetectionError
    };

    explicit DeckStore(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    [[nodiscard]] std::optional<DeckMutationErrorEnum> CreateRootDeck(const QString&, quint8);
    [[nodiscard]] std::optional<DeckMutationErrorEnum> CreateChildDeck(const QString&, const QString&);
    [[nodiscard]] std::optional<DeckMutationErrorEnum> MoveDeck(const QString&, const QString& = QString{});
    [[nodiscard]] std::optional<DeckMutationErrorEnum> UpdateDeckName(const QString&, const QString&);
    void DeleteDeck(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;

    [[nodiscard]] std::optional<DeckMutationErrorEnum> HandleRecoverableDeckMutationError(const std::unique_ptr<duckdb::QueryResult>&) const;
};

}
