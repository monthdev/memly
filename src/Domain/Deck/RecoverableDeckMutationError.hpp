#pragma once

#include <cstdint>

namespace Domain::Deck {

enum class RecoverableDeckMutationErrorEnum : std::uint8_t {
    InvalidDeckIdError,
    InvalidParentDeckIdError,
    DeckNameLengthError,
    DuplicateSiblingDeckNameError,
    InvalidTargetLanguageCodeError,
    ParentDeckTargetLanguageMismatchError,
    DeckTreeCycleDetectionError
};

}
