#pragma once

#include <cstdint>

namespace Application::Domain::ReviewSession {

enum class [[nodiscard]] RecoverableReviewSessionMutationErrorEnum : std::uint8_t {
    ReviewSessionNameLengthError,
    DuplicateReviewSessionDefinitionKeyError,
    ConflictingReviewSessionDeckSelfSelectionError,
    ConflictingReviewSessionDeckSubtreeSelectionError,
    ConflictingReviewSessionDeckIncludeSelectionError,
    ConflictingReviewSessionDeckExcludeSelectionError
};

}
