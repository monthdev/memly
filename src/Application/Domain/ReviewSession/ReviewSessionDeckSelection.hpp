#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace Application::Domain::ReviewSession {

struct ReviewSessionDeckSelection {
    enum class DeckSelectionTypeEnum : std::uint8_t {
        Self,
        Subtree,
        ExcludeSelf,
        ExcludeSubtree
    };

    std::string m_DeckId;
    DeckSelectionTypeEnum m_DeckSelectionType;

    explicit ReviewSessionDeckSelection(std::string&& DeckId, const DeckSelectionTypeEnum DeckSelectionType)
        : m_DeckId{ std::move(DeckId) }
        , m_DeckSelectionType{ DeckSelectionType } {
    }

    explicit ReviewSessionDeckSelection(const ReviewSessionDeckSelection&) = delete;
    explicit ReviewSessionDeckSelection(ReviewSessionDeckSelection&&) noexcept = default;
    auto operator=(const ReviewSessionDeckSelection&) -> ReviewSessionDeckSelection& = delete;
    auto operator=(ReviewSessionDeckSelection&&) -> ReviewSessionDeckSelection& = delete;
};

}
