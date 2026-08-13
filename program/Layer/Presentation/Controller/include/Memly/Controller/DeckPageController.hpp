// Temporarily disabled during deck page controller refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qobject.h>

#include <cstdint>
#include <expected>
#include <optional>
#include <string>

#include "Memly/Invalidation/LibraryInvalidationChannel.hpp"
#include "Memly/Model/DeckForestModel.hpp"

namespace Layer::Application::Service {
class DeckService;
}

namespace Layer::Presentation::Controller {

class DeckPageController final : public QObject {
    Q_OBJECT

private:
    Application::Service::DeckService& m_DeckService;
    Model::DeckForestModel m_DeckForestModel;

public:
    explicit DeckPageController(Application::Invalidation::LibraryInvalidationChannel&,
                                Application::Service::DeckService&,
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
