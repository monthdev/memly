#pragma once

#include <cstdint>
#include <optional>
#include <string>

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

    [[nodiscard]] std::optional<std::string> CreateRootDeck(const std::string&, std::uint8_t) noexcept;
    [[nodiscard]] std::optional<std::string> CreateChildDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::optional<std::string> MoveDeck(const std::string&, const std::optional<std::string>&) noexcept;
    [[nodiscard]] std::optional<std::string> RenameDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::optional<std::string> DeleteDeck(const std::string&) noexcept;

private:
    Application::Service::Deck::DeckService& m_DeckService;
};

}
