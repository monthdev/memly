#pragma once

#include <QString>
#include <QtTypes>
#include <optional>

namespace Application::Service::Deck {
class DeckService;
}

namespace Presentation::Controller::Deck {

class DeckController final {
public:
    explicit DeckController(Application::Service::Deck::DeckService& DeckService) noexcept
        : m_DeckService{ DeckService } {
    }

    ~DeckController() noexcept = default;
    DeckController(const DeckController&) = delete;
    DeckController(DeckController&&) = delete;
    DeckController& operator=(const DeckController&) = delete;
    DeckController& operator=(DeckController&&) = delete;

    [[nodiscard]] std::optional<QString> CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] std::optional<QString> CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] std::optional<QString> MoveDeck(const QString&, const std::optional<QString>&) noexcept;
    [[nodiscard]] std::optional<QString> RenameDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] std::optional<QString> DeleteDeck(const QString&) noexcept;

private:
    Application::Service::Deck::DeckService& m_DeckService;
};

}
