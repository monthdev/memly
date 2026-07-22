#pragma once

#include <unicode/unistr.h>

#include <cstddef>
#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Application::Domain::Text::Data {

class [[nodiscard]] HumanTextInput final : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
private:
    icu::UnicodeString m_NormalizedUnicodeString;

    explicit HumanTextInput(icu::UnicodeString&& NormalizedUnicodeString) noexcept
        : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
        , m_NormalizedUnicodeString{ std::move(NormalizedUnicodeString) } {
    }

public:
    [[nodiscard]] static auto FromInput(const std::string&) -> HumanTextInput;

    [[nodiscard]] auto ComputeGraphemeClusterLength() const -> std::size_t;
    [[nodiscard]] auto ToNormalizedStdString() const -> std::string;
    [[nodiscard]] auto ToNormalizedCaseFoldedStdString() && -> std::string;
};

}
