#pragma once

#include <cstdint>

namespace Domain::ReviewSession {

enum class RecoverableReviewSessionMutationErrorEnum : std::uint8_t {
    ReviewSessionNameLengthError,
    DuplicateReviewSessionDefinitionKeyError,
    ConflictingReviewSessionDeckSelfSelectionError,
    ConflictingReviewSessionDeckSubtreeSelectionError,
    ConflictingReviewSessionDeckIncludeSelectionError,
    ConflictingReviewSessionDeckExcludeSelectionError
};

}
