#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "Model/DeckTreeModel.hpp"
#include "Store/DeckHierarchyStore.hpp"

namespace Controller {

class DeckTreeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckTreeModel* deckTree READ GetDeckTree CONSTANT)

public:
    explicit DeckTreeController(Store::DeckHierarchyStore& DeckHierarchyStore,
                                QObject* Parent = nullptr) noexcept
        : QObject{ Parent }
        , m_DeckHierarchyStore{ DeckHierarchyStore }
        , m_DeckTreeRefreshTimer{}
        , m_DeckTree{ this } {
        m_DeckTreeRefreshTimer.setSingleShot(true);
        connect(&m_DeckTreeRefreshTimer, &QTimer::timeout, this, [this] { RefreshDeckTree(true); });
    }

    Model::DeckTreeModel* GetDeckTree() noexcept {
        return &m_DeckTree;
    }

    [[nodiscard]] Q_INVOKABLE QString NameLengthErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString DuplicateNameErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString TargetLanguageCodeErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString ParentDeckErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString CycleDetectionErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString&,
                                                 quint8,
                                                 const QString& = {}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = {}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString&, const QString&) noexcept;
    Q_INVOKABLE void DeleteDeck(const QString&) noexcept;
    Q_INVOKABLE void OnActivated();
    Q_INVOKABLE void OnDeactivated();

private:
    Store::DeckHierarchyStore& m_DeckHierarchyStore;
    QTimer m_DeckTreeRefreshTimer;
    Model::DeckTreeModel m_DeckTree;

    [[nodiscard]] QString
    HandleDeckMutationStatus(const Store::DeckHierarchyStore::DeckMutationStatus) const;
    void ScheduleNextDeckTreeRefresh() noexcept;
    void RefreshDeckTree(bool) noexcept;
};
}
