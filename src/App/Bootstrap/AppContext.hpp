#pragma once

#include "Service/DeckListService.hpp"

namespace App {

struct AppContext final {
    Service::DeckListService& m_DeckListService;
};

}
