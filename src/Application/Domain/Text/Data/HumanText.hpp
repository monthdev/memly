#pragma once

#include <string>
#include <utility>

namespace Application::Domain::Text::Data {

class [[nodiscard]] HumanText final {
public:
    [[nodiscard]] static HumanText FromPersisted(std::string&&, std::string&&) noexcept;
    HumanText(const HumanText&) = delete;
    HumanText(HumanText&&) noexcept = default;
    HumanText& operator=(const HumanText&) = delete;
    HumanText& operator=(HumanText&&) noexcept = default;

    [[nodiscard]] const std::string& GetNormalizedStdString() const noexcept;
    [[nodiscard]] const std::string& GetNormalizedCaseFoldedStdString() const noexcept;

private:
    explicit HumanText(std::string&& NormalizedText, std::string&& NormalizedCaseFoldedText) noexcept
        : m_NormalizedText{ std::move(NormalizedText) }
        , m_NormalizedCaseFoldedText{ std::move(NormalizedCaseFoldedText) } {
    }

    std::string m_NormalizedText;
    std::string m_NormalizedCaseFoldedText;
};

}
