#pragma once

#include <unicode/unistr.h>

#include <cstddef>
#include <string>

namespace Layer::Application::Domain {

class [[nodiscard]] HumanTextInput final {
private:
    icu::UnicodeString m_NormalizedUnicodeString;

    explicit HumanTextInput(icu::UnicodeString&&) noexcept;

public:
    explicit HumanTextInput(const HumanTextInput&) = delete;
    auto operator=(const HumanTextInput&) -> HumanTextInput& = delete;

    explicit HumanTextInput(HumanTextInput&&) noexcept = default;
    auto operator=(HumanTextInput&&) -> HumanTextInput& = delete;

    ~HumanTextInput() noexcept = default;

    [[nodiscard]] static auto FromInput(const std::string&) -> HumanTextInput;

    [[nodiscard]] auto ComputeGraphemeClusterLength() const -> std::size_t;
    [[nodiscard]] auto ToNormalizedStdString() const -> std::string;
    [[nodiscard]] auto ToNormalizedCaseFoldedStdString() && -> std::string;
};

}
