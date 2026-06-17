#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <string_view>

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

    [[nodiscard]] std::expected<void, std::string_view> CreateRootDeck(const std::string&, std::uint8_t) noexcept;
    [[nodiscard]] std::expected<void, std::string_view> CreateChildDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::expected<void, std::string_view> MoveDeck(const std::string&, const std::optional<std::string>&) noexcept;
    [[nodiscard]] std::expected<void, std::string_view> RenameDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::expected<void, std::string_view> DeleteDeck(const std::string&) noexcept;

private:
    Application::Service::Deck::DeckService& m_DeckService;
};

}
