#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace Domain::ReviewSession {

struct ReviewSessionDeckSelection {
    enum class SelectionTypeEnum : std::uint8_t {
        Self,
        Subtree,
        ExcludeSelf,
        ExcludeSubtree
    };

    ReviewSessionDeckSelection(std::string DeckId, const SelectionTypeEnum SelectionType)
        : m_DeckId{ std::move(DeckId) }
        , m_SelectionType{ SelectionType } {
    }

    ~ReviewSessionDeckSelection() = default;
    ReviewSessionDeckSelection(const ReviewSessionDeckSelection&) = delete;
    ReviewSessionDeckSelection(ReviewSessionDeckSelection&&) noexcept = default;
    ReviewSessionDeckSelection& operator=(const ReviewSessionDeckSelection&) = delete;
    ReviewSessionDeckSelection& operator=(ReviewSessionDeckSelection&&) noexcept = default;

    std::string m_DeckId;
    SelectionTypeEnum m_SelectionType;
};

}
