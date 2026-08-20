export module Memly.Controller:DeckPageController;

// Temporarily disabled during deck page controller refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <qobject.h>

#include <cstdint>
#include <expected>
#include <optional>
#include <string>

import Memly.Invalidation;
import Memly.Service;
import Memly.Model;

export namespace Memly::Controller {

class DeckPageController final : public QObject {
    Q_OBJECT

private:
    Service::DeckService& m_DeckService;
    Model::DeckForestModel m_DeckForestModel;

public:
    explicit DeckPageController(Invalidation::LibraryInvalidationChannel&,
                                Service::DeckService&,
                                QObject* = nullptr);

    explicit DeckPageController(const DeckPageController&) = delete;
    auto operator=(const DeckPageController&) -> DeckPageController& = delete;

    explicit DeckPageController(DeckPageController&&) = delete;
    auto operator=(DeckPageController&&) -> DeckPageController& = delete;

    ~DeckPageController() noexcept override = default;

    [[nodiscard]] auto GetDeckForestModel() noexcept -> Model::DeckForestModel*;

    [[nodiscard]] std::expected<void, const char*> CreateRootDeck(const std::string&, std::uint8_t) noexcept;
    [[nodiscard]] std::expected<void, const char*> CreateChildDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::expected<void, const char*> MoveDeck(const std::string&, const std::optional<std::string>&) noexcept;
    [[nodiscard]] std::expected<void, const char*> RenameDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::expected<void, const char*> DeleteDeck(const std::string&) noexcept;

private:
    void RefreshDeckForestModel(std::int64_t) noexcept;
};

}
#endif
