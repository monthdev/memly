#include "Layer/Application/Domain/Text/Data/HumanText.hpp"

#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Application::Domain::Text::Data {

HumanText::HumanText(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept
    : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
    , m_NormalizedText{ std::move(NormalizedText) }
    , m_NormalizedCaseFoldedText{ std::move(NormalizedCaseFoldedText) } {
}

[[nodiscard]] auto HumanText::FromPersisted(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept -> HumanText {
    return HumanText{ std::move(NormalizedText), std::move(NormalizedCaseFoldedText) };
}

[[nodiscard]] auto HumanText::GetNormalizedStdString() const noexcept -> const std::string& {
    return this->m_NormalizedText;
}

[[nodiscard]] auto HumanText::GetNormalizedCaseFoldedStdString() const noexcept -> const std::string& {
    return this->m_NormalizedCaseFoldedText;
}

}
