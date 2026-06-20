#pragma once

#include <QObject>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <string_view>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Presentation/Model/DeckTreeModel.hpp"

namespace Application::Service::Deck {
class DeckService;
class DeckTreeSnapshotService;
}

namespace Presentation::Controller {

class DeckPageController final : public QObject {
    Q_OBJECT

public:
    explicit DeckPageController(Application::Invalidation::LibraryInvalidationChannel& LibraryInvalidationChannel,
                                Application::Service::Deck::DeckService& DeckService,
                                Application::Service::Deck::DeckTreeSnapshotService& DeckTreeSnapshotService,
                                QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckService{ DeckService }
        , m_DeckTreeSnapshotService{ DeckTreeSnapshotService }
        , m_DeckTreeModel{ this } {
        LibraryInvalidationChannel.ConnectSnapshot(
            this, Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot, &DeckPageController::RefreshDeckTreeModel);
    }

    ~DeckPageController() noexcept override = default;
    DeckPageController(const DeckPageController&) = delete;
    DeckPageController(DeckPageController&&) = delete;
    DeckPageController& operator=(const DeckPageController&) = delete;
    DeckPageController& operator=(DeckPageController&&) = delete;

    [[nodiscard]] Presentation::Model::DeckTreeModel* GetDeckTreeModel() noexcept {
        return &m_DeckTreeModel;
    }

    [[nodiscard]] std::expected<void, std::string_view> CreateRootDeck(const std::string&, std::uint8_t) noexcept;
    [[nodiscard]] std::expected<void, std::string_view> CreateChildDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::expected<void, std::string_view> MoveDeck(const std::string&, const std::optional<std::string>&) noexcept;
    [[nodiscard]] std::expected<void, std::string_view> RenameDeck(const std::string&, const std::string&) noexcept;
    [[nodiscard]] std::expected<void, std::string_view> DeleteDeck(const std::string&) noexcept;

private:
    Application::Service::Deck::DeckService& m_DeckService;
    Application::Service::Deck::DeckTreeSnapshotService& m_DeckTreeSnapshotService;
    Presentation::Model::DeckTreeModel m_DeckTreeModel;

    void RefreshDeckTreeModel(std::int64_t) noexcept;
};

}
