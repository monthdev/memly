#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "Infrastructure/Store/DeckHierarchyStore.hpp"
#include "Infrastructure/Store/DeckStore.hpp"
#include "Presentation/Model/DeckTreeTableModel.hpp"

namespace Presentation::Controller {

class DeckTreeTableController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::DeckTreeTableModel* deckTreeTable READ GetDeckTreeTable CONSTANT)

public:
    explicit DeckTreeTableController(Infrastructure::Store::DeckHierarchyStore& DeckHierarchyStore,
                                     Infrastructure::Store::DeckStore& DeckStore,
                                     QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckHierarchyStore{ DeckHierarchyStore }
        , m_DeckStore{ DeckStore }
        , m_DeckTreeRefreshQTimer{}
        , m_DeckTreeTable{ this } {
        m_DeckTreeRefreshQTimer.setSingleShot(true);
        connect(&m_DeckTreeRefreshQTimer, &QTimer::timeout, this, [this] { RefreshDeckTreeTable(true); });
    }

    Model::DeckTreeTableModel* GetDeckTreeTable() noexcept {
        return &m_DeckTreeTable;
    }

    [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString&, quint8, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString&, const QString&) noexcept;
    Q_INVOKABLE void DeleteDeck(const QString&) noexcept;

    Q_INVOKABLE void OnActivated() noexcept;
    Q_INVOKABLE void OnDeactivated();

private:
    Infrastructure::Store::DeckHierarchyStore& m_DeckHierarchyStore;
    Infrastructure::Store::DeckStore& m_DeckStore;
    QTimer m_DeckTreeRefreshQTimer;
    Model::DeckTreeTableModel m_DeckTreeTable;

    [[nodiscard]] QString GetNameLengthErrorMessage() const;
    [[nodiscard]] QString GetDuplicateNameErrorMessage() const;
    [[nodiscard]] QString GetTargetLanguageCodeErrorMessage() const;
    [[nodiscard]] QString GetParentDeckErrorMessage() const;
    [[nodiscard]] QString GetCycleDetectionErrorMessage() const;
    [[nodiscard]] QString HandleDeckMutationStatus(const Infrastructure::Store::DeckStore::DeckMutationStatus) const;
    void ScheduleNextDeckTreeTableRefresh(const std::optional<std::int64_t>&);
    void RefreshDeckTreeTable(bool) noexcept;
};
}
