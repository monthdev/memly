#include "Application/Domain/Text/Data/HumanText.hpp"

#include <string>
#include <utility>

namespace Application::Domain::Text::Data {

[[nodiscard]] HumanText HumanText::FromPersisted(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept {
    return HumanText{ std::move(NormalizedText), std::move(NormalizedCaseFoldedText) };
}

[[nodiscard]] const std::string& HumanText::GetNormalizedStdString() const noexcept {
    return m_NormalizedText;
}

[[nodiscard]] const std::string& HumanText::GetNormalizedCaseFoldedStdString() const noexcept {
    return m_NormalizedCaseFoldedText;
}

}
