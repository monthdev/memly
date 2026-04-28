#pragma once

#include <QVector>

#include "Infrastructure/Store/DeckHierarchyStore.hpp"

namespace Infrastructure::Store {

void ValidatePreTreeDeckHierarchy(const QVector<DeckHierarchyStore::DeckHierarchyRow>&);

}
