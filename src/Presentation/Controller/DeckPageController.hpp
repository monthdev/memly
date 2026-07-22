// Temporarily disabled during deck page controller refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qobject.h>

#include <cstdint>
#include <expected>
#include <optional>
#include <string>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Presentation/Model/DeckForestModel.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Application::Service::Deck {
class DeckService;
}

namespace Presentation::Controller {

class DeckPageController final : public QObject, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    Q_OBJECT

private:
    Application::Service::Deck::DeckService& m_DeckService;
    Presentation::Model::DeckForestModel m_DeckForestModel;

public:
    explicit DeckPageController(Application::Invalidation::LibraryInvalidationChannel& LibraryInvalidationChannel,
                                Application::Service::Deck::DeckService& DeckService,
                                QObject* Parent = nullptr)
        : QObject{ Parent }
        , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DeckService{ DeckService }
        , m_DeckForestModel{ this } {
        LibraryInvalidationChannel.ConnectSnapshot(
            this, Application::Invalidation::LibraryInvalidationTargetEnum::DeckForestSnapshot, &DeckPageController::RefreshDeckForestModel);
    }
    [[nodiscard]] auto GetDeckForestModel() noexcept -> Presentation::Model::DeckForestModel*;

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
