#pragma once

#include <unicode/unistr.h>
#include <unicode/uversion.h>

#include <cstddef>
#include <string>

#include "Memly/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Application::Domain {

class [[nodiscard]] HumanTextInput final : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
private:
    icu::UnicodeString m_NormalizedUnicodeString;

    explicit HumanTextInput(icu::UnicodeString&&) noexcept;

public:
    [[nodiscard]] static auto FromInput(const std::string&) -> HumanTextInput;

    [[nodiscard]] auto ComputeGraphemeClusterLength() const -> std::size_t;
    [[nodiscard]] auto ToNormalizedStdString() const -> std::string;
    [[nodiscard]] auto ToNormalizedCaseFoldedStdString() && -> std::string;
};

}
