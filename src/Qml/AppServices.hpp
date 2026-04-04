#pragma once

#include <QtGlobal>

#include "Service/DeckListService.hpp"

namespace Qml {

class AppServices final {
public:
    static void InitializeDeckListService(Service::DeckListService* DeckListServiceInstance) noexcept {
        Q_ASSERT(DeckListServiceInstance != nullptr);
        Q_ASSERT(s_DeckListService == nullptr);
        s_DeckListService = DeckListServiceInstance;
    }

    static Service::DeckListService& GetRequiredDeckListService() noexcept {
        Q_ASSERT(s_DeckListService != nullptr);
        return *s_DeckListService;
    }

private:
    inline static Service::DeckListService* s_DeckListService{ nullptr };
};

}
