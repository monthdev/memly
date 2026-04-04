#pragma once

#include <QtGlobal>

#include "App/Bootstrap/AppContext.hpp"

namespace Qml {

class AppServices final {
public:
    static void Initialize(App::AppContext* AppContextInstance) noexcept {
        Q_ASSERT(AppContextInstance != nullptr);
        Q_ASSERT(s_AppContext == nullptr);
        s_AppContext = AppContextInstance;
    }

    static App::AppContext& GetRequired() noexcept {
        Q_ASSERT(s_AppContext != nullptr);
        return *s_AppContext;
    }

private:
    inline static App::AppContext* s_AppContext{ nullptr };
};

}
