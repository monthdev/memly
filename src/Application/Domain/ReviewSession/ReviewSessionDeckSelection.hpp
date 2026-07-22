#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Application::Domain::ReviewSession {

struct ReviewSessionDeckSelection : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
    enum class [[nodiscard]] DeckSelectionTypeEnum : std::uint8_t {
        Self,
        Subtree,
        ExcludeSelf,
        ExcludeSubtree
    };

    std::string m_DeckId;
    DeckSelectionTypeEnum m_DeckSelectionType;

    explicit ReviewSessionDeckSelection(std::string&& DeckId, const DeckSelectionTypeEnum DeckSelectionType)
        : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
        , m_DeckId{ std::move(DeckId) }
        , m_DeckSelectionType{ DeckSelectionType } {
    }
};

}
