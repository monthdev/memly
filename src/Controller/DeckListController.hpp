#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "Model/DeckListModel.hpp"
#include "Store/DeckListStore.hpp"

namespace Controller {

class DeckListController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckListModel* deckList READ GetDeckList CONSTANT)

public:
    explicit DeckListController(Store::DeckListStore& DeckListStore,
                                QObject* Parent = nullptr) noexcept
        : QObject{ Parent }
        , m_DeckListStore{ DeckListStore }
        , m_DeckListRefreshTimer{}
        , m_DeckList{ this } {
        m_DeckListRefreshTimer.setSingleShot(true);
        connect(&m_DeckListRefreshTimer, &QTimer::timeout, this, [this] { RefreshDeckList(true); });
    }

    Model::DeckListModel* GetDeckList() noexcept {
        return &m_DeckList;
    }

    [[nodiscard]] Q_INVOKABLE QString NameLengthErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString DuplicateNameErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString TargetLanguageCodeErrorMessage() const;
    [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString&, quint8) noexcept;
    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString&, const QString&) noexcept;
    Q_INVOKABLE void DeleteDeck(const QString&) noexcept;
    Q_INVOKABLE void OnActivated();
    Q_INVOKABLE void OnDeactivated();

private:
    Store::DeckListStore& m_DeckListStore;
    QTimer m_DeckListRefreshTimer;
    Model::DeckListModel m_DeckList;

    [[nodiscard]] QString
    HandleDeckMutationStatus(const Store::DeckListStore::DeckMutationStatus) const;
    void ScheduleNextDeckListRefresh() noexcept;
    void RefreshDeckList(bool) noexcept;
};
}
