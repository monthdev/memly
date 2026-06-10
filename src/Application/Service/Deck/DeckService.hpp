#pragma once

#include <QString>
#include <QtTypes>
#include <cstdint>
#include <expected>
#include <optional>

namespace Infrastructure::Sql {
class TransactionRunner;
}

namespace Infrastructure::Store::Deck {
class DeckStore;
}

namespace Application::Invalidation {
class LibraryInvalidationCoordinator;
}

namespace Application::Service::Deck {

class DeckService final {
public:
    enum class DeckMutationErrorEnum : std::uint8_t {
        DeckNameLengthError,
        DuplicateSiblingDeckNameError,
        InvalidTargetLanguageCodeError,
        ParentDeckTargetLanguageMismatchError,
        DeckTreeCycleDetectionError
    };

    DeckService(Infrastructure::Sql::TransactionRunner& TransactionRunner,
                Application::Invalidation::LibraryInvalidationCoordinator& LibraryInvalidationCoordinator,
                Infrastructure::Store::Deck::DeckStore& DeckStore) noexcept
        : m_TransactionRunner{ TransactionRunner }
        , m_LibraryInvalidationCoordinator{ LibraryInvalidationCoordinator }
        , m_DeckStore{ DeckStore } {
    }

    ~DeckService() = default;

    [[nodiscard]] std::expected<void, DeckMutationErrorEnum> CreateRootDeck(const QString&, quint8);
    [[nodiscard]] std::expected<void, DeckMutationErrorEnum> CreateChildDeck(const QString&, const QString&);
    [[nodiscard]] std::expected<void, DeckMutationErrorEnum> MoveDeck(const QString&, const std::optional<QString>&);
    [[nodiscard]] std::expected<void, DeckMutationErrorEnum> RenameDeck(const QString&, const QString&);
    [[nodiscard]] std::expected<void, DeckMutationErrorEnum> DeleteDeck(const QString&);

private:
    Infrastructure::Sql::TransactionRunner& m_TransactionRunner;
    Application::Invalidation::LibraryInvalidationCoordinator& m_LibraryInvalidationCoordinator;
    Infrastructure::Store::Deck::DeckStore& m_DeckStore;
};

}
