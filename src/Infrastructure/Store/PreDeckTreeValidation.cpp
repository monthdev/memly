#include "Infrastructure/Store/PreDeckTreeValidation.hpp"

#include <QHash>
#include <QSet>

#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

namespace {

void ValidateUniqueSiblingDeckNames(const QVector<DeckTreeStore::DeckTreeRow>& DeckTreeRowQVector) {
    QHash<QString, QSet<QString>> DeckNamesQSetByParentDeckIdQHash;
    DeckNamesQSetByParentDeckIdQHash.reserve(DeckTreeRowQVector.size());
    for (const DeckTreeStore::DeckTreeRow& DeckTreeRow : DeckTreeRowQVector) {
        QSet<QString>& DeckNamesQSet{ DeckNamesQSetByParentDeckIdQHash[DeckTreeRow.m_ParentDeckId] };
        if (DeckNamesQSet.contains(DeckTreeRow.m_DeckName)) {
            Support::ThrowError("Duplicate sibling deck name in deck tree snapshot");
        }
        DeckNamesQSet.insert(DeckTreeRow.m_DeckName);
    }
}

void ValidateParentChildTargetLanguageCodes(const QVector<DeckTreeStore::DeckTreeRow>& DeckTreeRowQVector) {
    QHash<QString, quint8> TargetLanguageCodeByDeckIdQHash;
    TargetLanguageCodeByDeckIdQHash.reserve(DeckTreeRowQVector.size());
    for (const DeckTreeStore::DeckTreeRow& DeckTreeRow : DeckTreeRowQVector) {
        TargetLanguageCodeByDeckIdQHash.insert(DeckTreeRow.m_DeckId, DeckTreeRow.m_TargetLanguageCode);
    }
    for (const DeckTreeStore::DeckTreeRow& DeckTreeRow : DeckTreeRowQVector) {
        if (DeckTreeRow.m_ParentDeckId.isEmpty()) {
            continue;
        }
        const auto& ParentTargetLanguageCodeByDeckIdQHashIterator{ TargetLanguageCodeByDeckIdQHash.constFind(DeckTreeRow.m_ParentDeckId) };
        if (ParentTargetLanguageCodeByDeckIdQHashIterator == TargetLanguageCodeByDeckIdQHash.cend()) {
            continue;
        }
        if (DeckTreeRow.m_TargetLanguageCode not_eq ParentTargetLanguageCodeByDeckIdQHashIterator.value()) {
            Support::ThrowError("Parent and child deck target language mismatch in deck tree snapshot");
        }
    }
}

}

void ValidatePreDeckTree(const QVector<DeckTreeStore::DeckTreeRow>& DeckTreeRowQVector) {
    ValidateUniqueSiblingDeckNames(DeckTreeRowQVector);
    ValidateParentChildTargetLanguageCodes(DeckTreeRowQVector);
}

}
