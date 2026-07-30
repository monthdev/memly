// Temporarily disabled during review session control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
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
#endif
