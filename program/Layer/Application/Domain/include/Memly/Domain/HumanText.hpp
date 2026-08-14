#pragma once

#include <string>

namespace Memly::Domain {

class [[nodiscard]] HumanText final {
private:
    std::string m_NormalizedText;
    std::string m_NormalizedCaseFoldedText;

    explicit HumanText(std::string&&, std::string&&) noexcept;

public:
    explicit HumanText(const HumanText&) = delete;
    auto operator=(const HumanText&) -> HumanText& = delete;

    explicit HumanText(HumanText&&) noexcept = default;
    auto operator=(HumanText&&) -> HumanText& = delete;

    ~HumanText() noexcept = default;

    [[nodiscard]] static auto FromPersisted(std::string&&, std::string&&) noexcept -> HumanText;

    [[nodiscard]] auto GetNormalizedStdString() const noexcept -> const std::string&;
    [[nodiscard]] auto GetNormalizedCaseFoldedStdString() const noexcept -> const std::string&;
};

}
