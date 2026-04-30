#pragma once

#include <QVector>

#include "Infrastructure/Store/DeckTreeStore.hpp"

namespace Infrastructure::Store {

void ValidatePreDeckTree(const QVector<DeckTreeStore::DeckTreeRow>&);

}
