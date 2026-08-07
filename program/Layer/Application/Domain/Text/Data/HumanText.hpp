#pragma once

#include <string>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Application::Domain::Text::Data {

class [[nodiscard]] HumanText final : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
private:
    std::string m_NormalizedText;
    std::string m_NormalizedCaseFoldedText;

    explicit HumanText(std::string&&, std::string&&) noexcept;

public:
    [[nodiscard]] static auto FromPersisted(std::string&&, std::string&&) noexcept -> HumanText;

    [[nodiscard]] auto GetNormalizedStdString() const noexcept -> const std::string&;
    [[nodiscard]] auto GetNormalizedCaseFoldedStdString() const noexcept -> const std::string&;
};

}
