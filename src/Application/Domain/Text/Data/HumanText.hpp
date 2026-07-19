#pragma once

#include <string>
#include <utility>

namespace Application::Domain::Text::Data {

class [[nodiscard]] HumanText final {
private:
    std::string m_NormalizedText;
    std::string m_NormalizedCaseFoldedText;

    explicit HumanText(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept
        : m_NormalizedText{ std::move(NormalizedText) }
        , m_NormalizedCaseFoldedText{ std::move(NormalizedCaseFoldedText) } {
    }

public:
    explicit HumanText(const HumanText&) = delete;
    explicit HumanText(HumanText&&) noexcept = default;
    auto operator=(const HumanText&) -> HumanText& = delete;
    auto operator=(HumanText&&) -> HumanText& = delete;

    [[nodiscard]] static auto FromPersisted(std::string&&, std::string&&) noexcept -> HumanText;

    [[nodiscard]] auto GetNormalizedStdString() const noexcept -> const std::string&;
    [[nodiscard]] auto GetNormalizedCaseFoldedStdString() const noexcept -> const std::string&;
};

}
