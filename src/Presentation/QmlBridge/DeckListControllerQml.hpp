#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>

#include "Controller/DeckListController.hpp"
#include "App/Bootstrap/AppServices.hpp"

namespace Qml {

class DeckListControllerQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckListModel* deckList READ GetDeckList CONSTANT)
    QML_NAMED_ELEMENT(DeckListController)

public:
    explicit DeckListControllerQml(QObject* Parent = nullptr) noexcept
        : QObject{ Parent }
        , m_DeckListController{ AppServices::GetRequiredDeckListService() } {
    }

    [[nodiscard]] Model::DeckListModel* GetDeckList() noexcept {
        return m_DeckListController.GetDeckList();
    }

    [[nodiscard]] Q_INVOKABLE QString NameLengthErrorMessage() const {
        return m_DeckListController.NameLengthErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString DuplicateNameErrorMessage() const {
        return m_DeckListController.DuplicateNameErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString TargetLanguageCodeErrorMessage() const {
        return m_DeckListController.TargetLanguageCodeErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString& DeckName,
                                                 const quint8 TargetLanguageCode) noexcept {
        return m_DeckListController.CreateDeck(DeckName, TargetLanguageCode);
    }

    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString& DeckId,
                                                     const QString& DeckName) noexcept {
        return m_DeckListController.UpdateDeckName(DeckId, DeckName);
    }

    Q_INVOKABLE void onActivated() {
        m_DeckListController.onActivated();
    }

    Q_INVOKABLE void onDeactivated() {
        m_DeckListController.onDeactivated();
    }

    Q_INVOKABLE void DeleteDeck(const QString& DeckId) noexcept {
        m_DeckListController.DeleteDeck(DeckId);
    }

private:
    Controller::DeckListController m_DeckListController;
};

}
