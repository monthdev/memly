#pragma once

#include <QObject>
#include <QString>
#include <QtTypes>
#include <optional>

#include "Coordinator/LibraryRefreshCoordinator.hpp"
#include "Infrastructure/Store/DeckStore.hpp"
#include "Infrastructure/Store/DeckTreeStore.hpp"
#include "Presentation/Model/DeckTreeTableModel.hpp"

namespace Presentation::Controller {

class DeckTreeTableController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::DeckTreeTableModel* deckTreeTable READ GetDeckTreeTable CONSTANT)

public:
    explicit DeckTreeTableController(Coordinator::LibraryRefreshCoordinator& LibraryRefreshCoordinator,
                                     Infrastructure::Store::DeckStore& DeckStore,
                                     Infrastructure::Store::DeckTreeStore& DeckTreeStore,
                                     QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_LibraryRefreshCoordinator{ LibraryRefreshCoordinator }
        , m_DeckStore{ DeckStore }
        , m_DeckTreeStore{ DeckTreeStore }
        , m_DeckTreeTable{ this } {
        connect(&m_LibraryRefreshCoordinator, &Coordinator::LibraryRefreshCoordinator::RefreshRequested, this, [this](const qint64 AsOfMillisecondsSinceEpoch) {
            RefreshDeckTreeTable(AsOfMillisecondsSinceEpoch);
        });
        RefreshDeckTreeTable(m_LibraryRefreshCoordinator.GetAsOfMillisecondsSinceEpoch());
    }

    Model::DeckTreeTableModel* GetDeckTreeTable() noexcept {
        return &m_DeckTreeTable;
    }

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString&, const QString&) noexcept;
    Q_INVOKABLE void DeleteDeck(const QString&) noexcept;

private:
    Coordinator::LibraryRefreshCoordinator& m_LibraryRefreshCoordinator;
    Infrastructure::Store::DeckStore& m_DeckStore;
    Infrastructure::Store::DeckTreeStore& m_DeckTreeStore;
    Model::DeckTreeTableModel m_DeckTreeTable;

    [[nodiscard]] QString GetNameLengthErrorMessage() const;
    [[nodiscard]] QString GetDuplicateNameErrorMessage() const;
    [[nodiscard]] QString GetTargetLanguageCodeErrorMessage() const;
    [[nodiscard]] QString GetParentDeckErrorMessage() const;
    [[nodiscard]] QString GetParentDeckTargetLanguageMismatchErrorMessage() const;
    [[nodiscard]] QString GetCycleDetectionErrorMessage() const;
    [[nodiscard]] QString HandleDeckMutationError(const std::optional<Infrastructure::Store::DeckStore::DeckMutationErrorEnum>) const;
    void RefreshDeckTreeTable(const qint64) noexcept;
};
}
