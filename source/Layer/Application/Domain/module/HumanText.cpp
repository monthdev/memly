module;

#include <string>
#include <string_view>
#include <utility>

module Memly.Domain.HumanText;

namespace Memly::Domain {

HumanText::HumanText(
    std::string&& NormalizedText,
    std::string&& NormalizedCaseFoldedText
) noexcept :
    m_NormalizedText{ std::move(NormalizedText) },
    m_NormalizedCaseFoldedText{ std::move(NormalizedCaseFoldedText) } {
}

[[nodiscard]] HumanText
HumanText::FromPersisted(
    std::string&& NormalizedText,
    std::string&& NormalizedCaseFoldedText
) noexcept {
    return HumanText{ std::move(NormalizedText),
        std::move(NormalizedCaseFoldedText) };
}

[[nodiscard]] std::string_view
HumanText::GetNormalizedStdStringView() const& noexcept {
    return std::string_view{ this->m_NormalizedText };
}

[[nodiscard]] std::string_view
HumanText::GetNormalizedCaseFoldedStdStringView() const& noexcept {
    return std::string_view{ this->m_NormalizedCaseFoldedText };
}

}
