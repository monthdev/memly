module;

#include <string>

export module Memly.Domain.DeckConstraint;

export namespace Memly::Domain {

[[nodiscard]] bool
IsDeckNameLengthValid(const std::string&) noexcept;

}
