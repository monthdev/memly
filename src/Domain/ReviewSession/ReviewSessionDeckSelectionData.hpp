#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace Domain::ReviewSession {

struct ReviewSessionDeckSelectionData {
    enum class SelectionTypeEnum : std::uint8_t {
        Self,
        Subtree,
        ExcludeSelf,
        ExcludeSubtree
    };

    ReviewSessionDeckSelectionData(std::string DeckId, const SelectionTypeEnum SelectionType)
        : m_DeckId{ std::move(DeckId) }
        , m_SelectionType{ SelectionType } {
    }

    ~ReviewSessionDeckSelectionData() = default;
    ReviewSessionDeckSelectionData(const ReviewSessionDeckSelectionData&) = delete;
    ReviewSessionDeckSelectionData(ReviewSessionDeckSelectionData&&) noexcept = default;
    ReviewSessionDeckSelectionData& operator=(const ReviewSessionDeckSelectionData&) = delete;
    ReviewSessionDeckSelectionData& operator=(ReviewSessionDeckSelectionData&&) noexcept = default;

    std::string m_DeckId;
    SelectionTypeEnum m_SelectionType;
};

}
