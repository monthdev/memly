// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Application/Domain/ReviewSession/ReviewSessionDeckSelection.hpp"

#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Application::Domain::ReviewSession {

ReviewSessionDeckSelection::ReviewSessionDeckSelection(std::string&& DeckId, const DeckSelectionTypeEnum DeckSelectionType)
    : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
    , m_DeckId{ std::move(DeckId) }
    , m_DeckSelectionType{ DeckSelectionType } {
}

}
#endif
