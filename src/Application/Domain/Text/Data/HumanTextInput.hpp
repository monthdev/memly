#pragma once

#include <unicode/unistr.h>

#include <cstddef>
#include <string>
#include <utility>

namespace Application::Domain::Text::Data {

class [[nodiscard]] HumanTextInput final {
public:
    [[nodiscard]] static HumanTextInput FromInput(const std::string&);
    HumanTextInput(const HumanTextInput&) = delete;
    HumanTextInput(HumanTextInput&&) noexcept = default;
    HumanTextInput& operator=(const HumanTextInput&) = delete;
    HumanTextInput& operator=(HumanTextInput&&) noexcept = default;

    [[nodiscard]] std::size_t ComputeGraphemeClusterLength() const;
    [[nodiscard]] std::string ToNormalizedStdString() const;
    [[nodiscard]] std::string ToNormalizedCaseFoldedStdString() &&;

private:
    explicit HumanTextInput(icu::UnicodeString&& NormalizedUnicodeString) noexcept
        : m_NormalizedUnicodeString{ std::move(NormalizedUnicodeString) } {
    }

    icu::UnicodeString m_NormalizedUnicodeString;
};

}
