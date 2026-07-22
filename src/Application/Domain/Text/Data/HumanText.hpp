#pragma once

#include <string>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Application::Domain::Text::Data {

class [[nodiscard]] HumanText final : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
private:
    std::string m_NormalizedText;
    std::string m_NormalizedCaseFoldedText;

    explicit HumanText(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept
        : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
        , m_NormalizedText{ std::move(NormalizedText) }
        , m_NormalizedCaseFoldedText{ std::move(NormalizedCaseFoldedText) } {
    }

public:
    [[nodiscard]] static auto FromPersisted(std::string&&, std::string&&) noexcept -> HumanText;

    [[nodiscard]] auto GetNormalizedStdString() const noexcept -> const std::string&;
    [[nodiscard]] auto GetNormalizedCaseFoldedStdString() const noexcept -> const std::string&;
};

}
