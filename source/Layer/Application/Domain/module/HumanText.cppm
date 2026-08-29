module;

#include <string>
#include <string_view>

export module Memly.Domain.HumanText;

export namespace Memly::Domain {

class [[nodiscard]] HumanText final {
private:
    std::string m_NormalizedText;
    std::string m_NormalizedCaseFoldedText;

    explicit HumanText(std::string&&, std::string&&) noexcept;

public:
    explicit HumanText(const HumanText&) = delete;
    HumanText&
    operator=(const HumanText&) = delete;

    explicit HumanText(HumanText&&) noexcept = default;
    HumanText&
    operator=(HumanText&&) = delete;

    ~HumanText() noexcept = default;

    [[nodiscard]] static HumanText
    FromPersisted(std::string&&, std::string&&) noexcept;

    [[nodiscard]] std::string_view
    GetNormalizedStdStringView() const& noexcept [[clang::lifetimebound]];

    [[nodiscard]] std::string_view
    GetNormalizedStdStringView() const&& noexcept = delete;

    [[nodiscard]] std::string_view
    GetNormalizedCaseFoldedStdStringView() const& noexcept
        [[clang::lifetimebound]];
    [[nodiscard]] std::string_view
    GetNormalizedCaseFoldedStdStringView() const&& noexcept = delete;
};

}
