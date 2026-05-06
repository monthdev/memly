#pragma once

#include <QVector>

#include "Infrastructure/Store/DeckTreeStore.hpp"

namespace Infrastructure::Store {

void ValidateFlatDeckTree(const QVector<DeckTreeStore::DeckTreeRow>&);

}
