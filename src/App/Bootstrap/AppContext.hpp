#pragma once

#include "Store/DeckListStore.hpp"

namespace App {

struct AppContext final {
    Store::DeckListStore& m_DeckListStore;
};

}
