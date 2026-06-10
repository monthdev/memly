#pragma once

#include <QObject>
#include <QString>
#include <QtTypes>
#include <optional>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Application/Service/Deck/DeckService.hpp"
#include "Application/Service/Deck/DeckTreeService.hpp"
#include "Presentation/Model/Deck/DeckTreeModel.hpp"

namespace Presentation::Controller::Deck {

class DeckTreeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::Deck::DeckTreeModel* deckTree READ GetDeckTree CONSTANT)

public:
    explicit DeckTreeController(Application::Invalidation::LibraryInvalidationChannel& LibraryInvalidationChannel,
                                Application::Service::Deck::DeckService& DeckService,
                                Application::Service::Deck::DeckTreeService& DeckTreeService,
                                QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_LibraryInvalidationChannel{ LibraryInvalidationChannel }
        , m_DeckService{ DeckService }
        , m_DeckTreeService{ DeckTreeService }
        , m_DeckTree{ this } {
        m_LibraryInvalidationChannel.ConnectSnapshot(
            this, Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot, &DeckTreeController::RefreshDeckTree);
    }

    ~DeckTreeController() override = default;

    Presentation::Model::Deck::DeckTreeModel* GetDeckTree() noexcept {
        return &m_DeckTree;
    }

    [[nodiscard]] std::optional<QString> CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] std::optional<QString> CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] std::optional<QString> MoveDeck(const QString&, const std::optional<QString>&) noexcept;
    [[nodiscard]] std::optional<QString> RenameDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] std::optional<QString> DeleteDeck(const QString&) noexcept;

private:
    Application::Invalidation::LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Application::Service::Deck::DeckService& m_DeckService;
    Application::Service::Deck::DeckTreeService& m_DeckTreeService;
    Presentation::Model::Deck::DeckTreeModel m_DeckTree;

    [[nodiscard]] QString DeckMutationErrorToQString(Application::Service::Deck::DeckService::DeckMutationErrorEnum) const;
    void RefreshDeckTree(const qint64) noexcept;
};
}
