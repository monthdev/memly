#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "Model/DeckTreeTableModel.hpp"
#include "Store/DeckHierarchyStore.hpp"

namespace Controller {

class DeckTreeTableController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckTreeTableModel* deckTree READ GetDeckTree CONSTANT)

public:
    explicit DeckTreeTableController(Store::DeckHierarchyStore& DeckHierarchyStore, QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckHierarchyStore{ DeckHierarchyStore }
        , m_DeckTreeRefreshTimer{}
        , m_DeckTree{ this } {
        m_DeckTreeRefreshTimer.setSingleShot(true);
        connect(&m_DeckTreeRefreshTimer, &QTimer::timeout, this, [this] { RefreshDeckTree(true); });
    }

    Model::DeckTreeTableModel* GetDeckTree() noexcept {
        return &m_DeckTree;
    }

    [[nodiscard]] Q_INVOKABLE QString GetNameLengthErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString GetDuplicateNameErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString GetTargetLanguageCodeErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString GetParentDeckErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString GetCycleDetectionErrorMessage() const;

    [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString&, quint8, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString&, const QString&) noexcept;
    Q_INVOKABLE void DeleteDeck(const QString&) noexcept;

    Q_INVOKABLE void OnActivated() noexcept;
    Q_INVOKABLE void OnDeactivated();

private:
    Store::DeckHierarchyStore& m_DeckHierarchyStore;
    QTimer m_DeckTreeRefreshTimer;
    Model::DeckTreeTableModel m_DeckTree;

    [[nodiscard]] QString HandleDeckMutationStatus(const Store::DeckHierarchyStore::DeckMutationStatus) const;
    void ScheduleNextDeckTreeRefresh(const std::optional<std::int64_t>&);
    void RefreshDeckTree(bool) noexcept;
};
}
