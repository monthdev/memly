#pragma once

#include <cstdint>

namespace Domain::Deck {

enum class RecoverableDeckIdErrorEnum : std::uint8_t {
    InvalidDeckIdError,
    InvalidParentDeckIdError
};

}
