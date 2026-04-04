#include "Controller/DeckListController.hpp"

#include <cstdint>

#include "Support/Fatal.hpp"

namespace Controller {

[[nodiscard]] Q_INVOKABLE QString DeckListController::NameLengthErrorMessage() const {
    return QString{ "Deck name length exceeds character limit" };
}

[[nodiscard]] Q_INVOKABLE QString DeckListController::DuplicateNameErrorMessage() const {
    return QString{ "Deck name already exists" };
}

[[nodiscard]] Q_INVOKABLE QString DeckListController::TargetLanguageCodeErrorMessage() const {
    return QString{ "Target language code is invalid" };
}

[[nodiscard]] QString DeckListController::HandleDeckMutationStatus(
    const Store::DeckListStore::DeckMutationStatus DeckMutationStatus) const {
    switch (DeckMutationStatus) {
    case Store::DeckListStore::DeckMutationStatus::Success: {
        return QString{};
    }
    case Store::DeckListStore::DeckMutationStatus::NameLengthError: {
        return NameLengthErrorMessage();
    }
    case Store::DeckListStore::DeckMutationStatus::DuplicateNameError: {
        return DuplicateNameErrorMessage();
    }
    case Store::DeckListStore::DeckMutationStatus::TargetLanguageCodeError: {
        return TargetLanguageCodeErrorMessage();
    }
    }
}

[[nodiscard]] Q_INVOKABLE QString
DeckListController::CreateDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
    return Support::TryCatchWrapper([&] {
        const QString ErrorMessage{ HandleDeckMutationStatus(
            m_DeckListStore.CreateDeck(DeckName, TargetLanguageCode)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckList(false);
        }
        return ErrorMessage;
    });
}

[[nodiscard]] Q_INVOKABLE QString
DeckListController::UpdateDeckName(const QString& DeckId, const QString& DeckName) noexcept {
    return Support::TryCatchWrapper([&] {
        const QString ErrorMessage{ HandleDeckMutationStatus(
            m_DeckListStore.UpdateDeckName(DeckId, DeckName)) };
        if (ErrorMessage.isEmpty()) {
            RefreshDeckList(false);
        }
        return ErrorMessage;
    });
}

Q_INVOKABLE void DeckListController::onActivated() {
    RefreshDeckList(true);
}

Q_INVOKABLE void DeckListController::onDeactivated() {
    m_DeckListRefreshTimer.stop();
}

Q_INVOKABLE void DeckListController::DeleteDeck(const QString& DeckId) noexcept {
    Support::TryCatchWrapper([&] {
        m_DeckListStore.DeleteDeck(DeckId);
        RefreshDeckList(true);
    });
}

void DeckListController::ScheduleNextDeckListRefresh() noexcept {
    Support::TryCatchWrapper([&] {
        m_DeckListRefreshTimer.stop();
        const std::optional<std::int64_t> DeckListNextRefreshDelayMilliseconds{
            m_DeckListStore.ReadDeckListNextRefreshDelayMilliseconds()
        };
        if (!DeckListNextRefreshDelayMilliseconds.has_value()) {
            return;
        }
        m_DeckListRefreshTimer.start(
            static_cast<int>(DeckListNextRefreshDelayMilliseconds.value()));
    });
}

void DeckListController::RefreshDeckList(bool NeedScheduledNextDeckListRefresh) noexcept {
    Support::TryCatchWrapper([&] {
        const QVector<Store::DeckListStore::DeckListRowData> DeckListRowDataVector{
            m_DeckListStore.ReadDeckList()
        };
        QVector<Model::DeckListModel::DeckListRow> DeckList;
        DeckList.reserve(DeckListRowDataVector.size());
        for (const auto& DeckListRowData : DeckListRowDataVector) {
            DeckList.emplace_back(DeckListRowData.m_Id,
                                  DeckListRowData.m_Name,
                                  DeckListRowData.m_DueNow,
                                  DeckListRowData.m_ByToday,
                                  DeckListRowData.m_Total);
        }
        m_DeckList.ReplaceAll(std::move(DeckList));
        if (NeedScheduledNextDeckListRefresh) {
            ScheduleNextDeckListRefresh();
        }
    });
}
}
