#pragma once

#include <QObject>
#include <QString>
#include <QtTypes>
#include <optional>

#include "Application/Coordinator/LibraryRefreshCoordinator.hpp"
#include "Infrastructure/Store/DeckStore.hpp"
#include "Infrastructure/Store/DeckTreeStore.hpp"
#include "Presentation/Model/DeckTreeModel.hpp"

namespace Presentation::Controller {

class DeckTreeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::DeckTreeModel* deckTree READ GetDeckTree CONSTANT)

public:
    explicit DeckTreeController(Application::Coordinator::LibraryRefreshCoordinator& LibraryRefreshCoordinator,
                                Infrastructure::Store::DeckStore& DeckStore,
                                Infrastructure::Store::DeckTreeStore& DeckTreeStore,
                                QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_LibraryRefreshCoordinator{ LibraryRefreshCoordinator }
        , m_DeckStore{ DeckStore }
        , m_DeckTreeStore{ DeckTreeStore }
        , m_DeckTree{ this } {
        connect(&m_LibraryRefreshCoordinator,
                &Application::Coordinator::LibraryRefreshCoordinator::RefreshRequested,
                this,
                [this](const qint64 AsOfMillisecondsSinceEpoch) { RefreshDeckTree(AsOfMillisecondsSinceEpoch); });
        RefreshDeckTree(m_LibraryRefreshCoordinator.GetAsOfMillisecondsSinceEpoch());
    }

    Model::DeckTreeModel* GetDeckTree() noexcept {
        return &m_DeckTree;
    }

    [[nodiscard]] std::optional<QString> CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] std::optional<QString> CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] std::optional<QString> MoveDeck(const QString&, const std::optional<QString>&) noexcept;
    [[nodiscard]] std::optional<QString> RenameDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] std::optional<QString> DeleteDeck(const QString&) noexcept;

private:
    Application::Coordinator::LibraryRefreshCoordinator& m_LibraryRefreshCoordinator;
    Infrastructure::Store::DeckStore& m_DeckStore;
    Infrastructure::Store::DeckTreeStore& m_DeckTreeStore;
    Model::DeckTreeModel m_DeckTree;

    [[nodiscard]] QString GetNameLengthErrorMessage() const;
    [[nodiscard]] QString GetDuplicateNameErrorMessage() const;
    [[nodiscard]] QString GetTargetLanguageCodeErrorMessage() const;
    [[nodiscard]] QString GetParentDeckTargetLanguageMismatchErrorMessage() const;
    [[nodiscard]] QString GetDeckTreeCycleDetectionErrorMessage() const;
    [[nodiscard]] std::optional<QString>
    RecoverableDeckMutationErrorToQString(const std::optional<Infrastructure::Store::DeckStore::RecoverableDeckMutationErrorEnum>) const;
    void RefreshDeckTree(const qint64) noexcept;
};
}
