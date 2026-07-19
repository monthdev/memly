#include "Application/Domain/Text/Data/HumanText.hpp"

#include <string>
#include <utility>

namespace Application::Domain::Text::Data {

[[nodiscard]] auto HumanText::FromPersisted(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept -> HumanText {
    return HumanText{ std::move(NormalizedText), std::move(NormalizedCaseFoldedText) };
}

[[nodiscard]] auto HumanText::GetNormalizedStdString() const noexcept -> const std::string& {
    return m_NormalizedText;
}

[[nodiscard]] auto HumanText::GetNormalizedCaseFoldedStdString() const noexcept -> const std::string& {
    return m_NormalizedCaseFoldedText;
}

}
