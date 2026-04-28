#include "Infrastructure/Store/PreTreeDeckHierarchyValidation.hpp"

#include <QHash>
#include <QSet>

#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

namespace {

void ValidateUniqueSiblingDeckNames(const QVector<DeckHierarchyStore::DeckHierarchyRow>& DeckHierarchyRowQVector) {
    QHash<QString, QSet<QString>> DeckNamesQSetByParentDeckIdQHash;
    DeckNamesQSetByParentDeckIdQHash.reserve(DeckHierarchyRowQVector.size());
    for (const DeckHierarchyStore::DeckHierarchyRow& DeckHierarchyRow : DeckHierarchyRowQVector) {
        QSet<QString>& DeckNamesQSet{ DeckNamesQSetByParentDeckIdQHash[DeckHierarchyRow.m_ParentDeckId] };
        if (DeckNamesQSet.contains(DeckHierarchyRow.m_Name)) {
            Support::ThrowError("Duplicate sibling deck name in deck hierarchy snapshot");
        }
        DeckNamesQSet.insert(DeckHierarchyRow.m_Name);
    }
}

void ValidateParentChildTargetLanguageCodes(const QVector<DeckHierarchyStore::DeckHierarchyRow>& DeckHierarchyRowQVector) {
    QHash<QString, quint8> TargetLanguageCodeByDeckIdQHash;
    TargetLanguageCodeByDeckIdQHash.reserve(DeckHierarchyRowQVector.size());
    for (const DeckHierarchyStore::DeckHierarchyRow& DeckHierarchyRow : DeckHierarchyRowQVector) {
        TargetLanguageCodeByDeckIdQHash.insert(DeckHierarchyRow.m_DeckId, DeckHierarchyRow.m_TargetLanguageCode);
    }
    for (const DeckHierarchyStore::DeckHierarchyRow& DeckHierarchyRow : DeckHierarchyRowQVector) {
        if (DeckHierarchyRow.m_ParentDeckId.isEmpty()) {
            continue;
        }
        const auto& ParentTargetLanguageCodeByDeckIdQHashIterator{ TargetLanguageCodeByDeckIdQHash.constFind(DeckHierarchyRow.m_ParentDeckId) };
        if (ParentTargetLanguageCodeByDeckIdQHashIterator == TargetLanguageCodeByDeckIdQHash.cend()) {
            continue;
        }
        if (DeckHierarchyRow.m_TargetLanguageCode not_eq ParentTargetLanguageCodeByDeckIdQHashIterator.value()) {
            Support::ThrowError("Parent and child deck target language mismatch in deck hierarchy snapshot");
        }
    }
}

}

void ValidatePreTreeDeckHierarchy(const QVector<DeckHierarchyStore::DeckHierarchyRow>& DeckHierarchyRowQVector) {
    ValidateUniqueSiblingDeckNames(DeckHierarchyRowQVector);
    ValidateParentChildTargetLanguageCodes(DeckHierarchyRowQVector);
}

}
