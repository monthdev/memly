#pragma once

#include <cstdint>

namespace Domain::Deck {

enum class RecoverableDeckMutationErrorEnum : std::uint8_t {
    DeckNameLengthError,
    DuplicateSiblingDeckNameError,
    InvalidTargetLanguageCodeError,
    ParentDeckTargetLanguageMismatchError,
    DeckTreeCycleDetectionError
};

}
