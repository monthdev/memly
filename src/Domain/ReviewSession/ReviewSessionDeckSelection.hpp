#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace Domain::ReviewSession {

struct ReviewSessionDeckSelection {
    enum class DeckSelectionTypeEnum : std::uint8_t {
        Self,
        Subtree,
        ExcludeSelf,
        ExcludeSubtree
    };

    ReviewSessionDeckSelection(std::string DeckId, const DeckSelectionTypeEnum DeckSelectionType)
        : m_DeckId{ std::move(DeckId) }
        , m_DeckSelectionType{ DeckSelectionType } {
    }

    ~ReviewSessionDeckSelection() = default;
    ReviewSessionDeckSelection(const ReviewSessionDeckSelection&) = delete;
    ReviewSessionDeckSelection(ReviewSessionDeckSelection&&) noexcept = default;
    ReviewSessionDeckSelection& operator=(const ReviewSessionDeckSelection&) = delete;
    ReviewSessionDeckSelection& operator=(ReviewSessionDeckSelection&&) noexcept = default;

    std::string m_DeckId;
    DeckSelectionTypeEnum m_DeckSelectionType;
};

}
