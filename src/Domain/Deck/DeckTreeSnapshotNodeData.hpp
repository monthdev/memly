#pragma once

#include <QString>
#include <QtTypes>
#include <optional>
#include <utility>

namespace Domain::Deck {

struct DeckTreeSnapshotNodeData {
    DeckTreeSnapshotNodeData(QString DeckId,
                             std::optional<QString> ParentDeckId,
                             QString DeckName,
                             qint64 CreatedAtMillisecondsSinceEpoch,
                             std::optional<qint64> UpdatedAtMillisecondsSinceEpoch,
                             quint32 SelfDueNowCount,
                             quint32 SelfByTodayCount,
                             quint32 SelfTotalCount,
                             quint32 SubtreeDueNowCount,
                             quint32 SubtreeByTodayCount,
                             quint32 SubtreeTotalCount,
                             quint8 TargetLanguageCode)
        : m_DeckId{ std::move(DeckId) }
        , m_ParentDeckId{ std::move(ParentDeckId) }
        , m_DeckName{ std::move(DeckName) }
        , m_CreatedAtMillisecondsSinceEpoch{ CreatedAtMillisecondsSinceEpoch }
        , m_UpdatedAtMillisecondsSinceEpoch{ std::move(UpdatedAtMillisecondsSinceEpoch) }
        , m_SelfDueNowCount{ SelfDueNowCount }
        , m_SelfByTodayCount{ SelfByTodayCount }
        , m_SelfTotalCount{ SelfTotalCount }
        , m_SubtreeDueNowCount{ SubtreeDueNowCount }
        , m_SubtreeByTodayCount{ SubtreeByTodayCount }
        , m_SubtreeTotalCount{ SubtreeTotalCount }
        , m_TargetLanguageCode{ TargetLanguageCode } {
    }

    ~DeckTreeSnapshotNodeData() = default;
    DeckTreeSnapshotNodeData(const DeckTreeSnapshotNodeData&) = delete;
    DeckTreeSnapshotNodeData(DeckTreeSnapshotNodeData&&) noexcept = default;
    DeckTreeSnapshotNodeData& operator=(const DeckTreeSnapshotNodeData&) = delete;
    DeckTreeSnapshotNodeData& operator=(DeckTreeSnapshotNodeData&&) noexcept = default;

    QString m_DeckId;
    std::optional<QString> m_ParentDeckId;
    QString m_DeckName;
    qint64 m_CreatedAtMillisecondsSinceEpoch;
    std::optional<qint64> m_UpdatedAtMillisecondsSinceEpoch;
    quint32 m_SelfDueNowCount;
    quint32 m_SelfByTodayCount;
    quint32 m_SelfTotalCount;
    quint32 m_SubtreeDueNowCount;
    quint32 m_SubtreeByTodayCount;
    quint32 m_SubtreeTotalCount;
    quint8 m_TargetLanguageCode;
};

}
