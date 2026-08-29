module;

#include <unicode/unistr.h>

#include <cstddef>
#include <string>

export module Memly.Domain.HumanTextInput;

export namespace Memly::Domain {

class [[nodiscard]] HumanTextInput final {
private:
    icu::UnicodeString m_NormalizedUnicodeString;

    explicit HumanTextInput(icu::UnicodeString&&) noexcept;

public:
    explicit HumanTextInput(const HumanTextInput&) = delete;
    HumanTextInput&
    operator=(const HumanTextInput&) = delete;

    explicit HumanTextInput(HumanTextInput&&) noexcept = default;
    HumanTextInput&
    operator=(HumanTextInput&&) = delete;

    ~HumanTextInput() noexcept = default;

    [[nodiscard]] static HumanTextInput
    FromInput(const std::string&);

    [[nodiscard]] std::size_t
    ComputeGraphemeClusterLength() const;

    [[nodiscard]] std::string
    ToNormalizedStdString() const;

    [[nodiscard]] std::string
    ToNormalizedCaseFoldedStdString() &&;
};

}
