#pragma once

#include <QVector>

#include "Infrastructure/Store/Deck/DeckTreeStore.hpp"

namespace Infrastructure::Store::Deck {

void ValidateFlatDeckTree(const QVector<DeckTreeStore::DeckTreeRow>&);

}
