#pragma once

#include <QString>
#include <QtGlobal>
#include <memory>

namespace duckdb {
class Connection;
class QueryResult;
}

namespace Infrastructure::Store {

class DeckStore final {
public:
    enum class DeckMutationStatus {
        Success,
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

    [[nodiscard]] DeckMutationStatus CreateRootDeck(const QString&, quint8);
    [[nodiscard]] DeckMutationStatus CreateChildDeck(const QString&, const QString&);
    [[nodiscard]] DeckMutationStatus MoveDeck(const QString&, const QString& = QString{});
    [[nodiscard]] DeckMutationStatus UpdateDeckName(const QString&, const QString&);
    void DeleteDeck(const QString&);

private:
    duckdb::Connection& m_DatabaseConnection;

    [[nodiscard]] DeckMutationStatus HandleRecoverableDeckMutationError(const std::unique_ptr<duckdb::QueryResult>&) const;
};

}
