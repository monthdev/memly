#pragma once

#include <unicode/unistr.h>

#include <cstddef>
#include <string>
#include <utility>

namespace Application::Domain::Text::Data {

class [[nodiscard]] HumanTextInput final {
private:
    icu::UnicodeString m_NormalizedUnicodeString;

    explicit HumanTextInput(icu::UnicodeString&& NormalizedUnicodeString) noexcept
        : m_NormalizedUnicodeString{ std::move(NormalizedUnicodeString) } {
    }

public:
    explicit HumanTextInput(const HumanTextInput&) = delete;
    explicit HumanTextInput(HumanTextInput&&) noexcept = default;
    auto operator=(const HumanTextInput&) -> HumanTextInput& = delete;
    auto operator=(HumanTextInput&&) -> HumanTextInput& = delete;

    [[nodiscard]] static auto FromInput(const std::string&) -> HumanTextInput;

    [[nodiscard]] auto ComputeGraphemeClusterLength() const -> std::size_t;
    [[nodiscard]] auto ToNormalizedStdString() const -> std::string;
    [[nodiscard]] auto ToNormalizedCaseFoldedStdString() && -> std::string;
};

}
