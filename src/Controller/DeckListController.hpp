#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "Model/DeckListModel.hpp"
#include "Service/DeckListService.hpp"

namespace Controller {

class DeckListController : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckListModel* deckList READ GetDeckList CONSTANT)

public:
    explicit DeckListController(Service::DeckListService& DeckListService,
                                QObject* Parent = nullptr) noexcept
        : QObject{ Parent }
        , m_DeckListService{ DeckListService }
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
    Q_INVOKABLE void onActivated();
    Q_INVOKABLE void onDeactivated();
    Q_INVOKABLE void DeleteDeck(const QString&) noexcept;

private:
    Service::DeckListService& m_DeckListService;
    QTimer m_DeckListRefreshTimer;
    Model::DeckListModel m_DeckList;

    [[nodiscard]] QString
    HandleDeckMutationStatus(const Service::DeckListService::DeckMutationStatus) const;
    void ScheduleNextDeckListRefresh() noexcept;
    void RefreshDeckList(bool) noexcept;
};
}
