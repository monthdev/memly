#include "Bridge/DeckBridge.hpp"

#include "Support/Fatal.hpp"

namespace Bridge {
Q_INVOKABLE QString DeckBridge::AddDeck(const QString& Name) {
    return Support::TryCatchWrapper([&] { return Name; });
}

}
