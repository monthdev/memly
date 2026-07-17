#if 0
// Temporarily disabled during deck page controller refactor.
#pragma once

#include <qobject.h>

#include <cstdint>
#include <expected>
#include <optional>
#include <string>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Presentation/Model/DeckForestModel.hpp"

namespace Application::Service::Deck {
class DeckService;
}

namespace Presentation::Controller {

class DeckPageController final : public QObject {
    Q_OBJECT

public:
    explicit DeckPageController(Application::Invalidation::LibraryInvalidationChannel& LibraryInvalidationChannel,
                                Application::Service::Deck::DeckService& DeckService,
                                QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckService{ DeckService }
        , m_DeckForestModel{ this } {
        LibraryInvalidationChannel.ConnectSnapshot(
            this, Application::Invalidation::LibraryInvalidationTargetEnum::DeckForestSnapshot, &DeckPageController::RefreshDeckForestModel);
    }
    DeckPageController(const DeckPageController&) = delete;
    DeckPageController(DeckPageController&&) = delete;
    DeckPageController& operator=(const DeckPageController&) = delete;
    DeckPageController& operator=(DeckPageController&&) = delete;

    [[nodiscard]] Presentation::Model::DeckForestModel* GetDeckForestModel() noexcept {
        return &m_DeckForestModel;
    }

    [[nodiscard]] std::expected<void, const char*> CreateRootDeck(const std::string&, std::uint8_t) noexcept;
    [[nodiscard]] std::expected<void, const char*> CreateChildDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::expected<void, const char*> MoveDeck(const std::string&, const std::optional<std::string>&) noexcept;
    [[nodiscard]] std::expected<void, const char*> RenameDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::expected<void, const char*> DeleteDeck(const std::string&) noexcept;

private:
    Application::Service::Deck::DeckService& m_DeckService;
    Presentation::Model::DeckForestModel m_DeckForestModel;

    void RefreshDeckForestModel(std::int64_t) noexcept;
};

}
#endif
