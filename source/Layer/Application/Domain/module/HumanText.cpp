module;

#include <string>
#include <string_view>
#include <utility>

module Memly.Domain.HumanText;

namespace Memly::Domain {

HumanText::HumanText(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept
    : m_NormalizedText{ std::move(NormalizedText) }
    , m_NormalizedCaseFoldedText{ std::move(NormalizedCaseFoldedText) } {
}

[[nodiscard]] auto HumanText::FromPersisted(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept -> HumanText {
    return HumanText{ std::move(NormalizedText), std::move(NormalizedCaseFoldedText) };
}

[[nodiscard]] auto HumanText::GetNormalizedStdStringView() const& noexcept -> std::string_view {
    return std::string_view{ this->m_NormalizedText };
}

[[nodiscard]] auto HumanText::GetNormalizedCaseFoldedStdStringView() const& noexcept -> std::string_view {
    return std::string_view{ this->m_NormalizedCaseFoldedText };
}

}
