module;

#include <string>

export module Memly.Domain.DeckConstraint;

export namespace Memly::Domain {

[[nodiscard]] auto IsDeckNameLengthValid(const std::string&) noexcept -> bool;

}
